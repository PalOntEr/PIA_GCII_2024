#ifndef _dxrr
#define _dxrr
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include <cmath>
#include "TerrenoRR.h"
#include "Water.h"
#include "Camara.h"
#include "Player.h"
#include "Enemy.h"
#include "Turret.h"
#include "SkyDome.h"
#include "Billboard.h"
#include "ModeloRR.h"
#include "XACT3Util.h"
#include "GUI.h"
#include "Text.h"
#define DAYCYCLESPEED 0.0001f/*0.0001f*/
#define GRAVITYFORCE -0.03f
#define GAMEDURATION 180.0f
#define GAMEENDS false
#define QUICKLOAD false

//MAX ANDRES ZERTUCHE PEREZ #2003051
//MATEO ZAMORA GRAJEDA #2001215
//CARLOS ALBERTO PECINA AGUIRRE #2025018

class DXRR{	

private:
	int ancho;
	int alto;
	bool vehicleMoved;
	float* prevPos;

public:	
	HINSTANCE hInstance;
	HWND hWnd;

	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferTarget;

	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilDisabledState;

	ID3D11BlendState *alphaBlendState, *commonBlendState;

	int frameBillboard;
	int frameSmoke;
	int globalTimer;
	int realTime;

	TerrenoRR* terreno;
	Water* water;
	SkyDome* skydome;
	BillboardRR* billboard;
	BillboardRR* smoke;
	BillboardRR* antSalesman;
	BillboardRR* rustySign;
	BillboardRR* mountains;
	BillboardRR* grass;
	int totalGrass;
	Player* player;
	Enemy** spiderEnemies;
	Turret*** sceneTurrets;
	int maxTurrets;
	int currentTurrets;
	int TurretTypes;
	
	ModeloRR*** sceneAssets;
	int totalAssets;

	float*** sceneModels;
	int totalModels;
	int totalTrees;
	float*** sceneTargets;
	int totalTargets;
	float** sceneVehicle;
	float** sceneAnthole;
	float** sceneWalls;
	int totalWalls;
	int totalLeaves;
	float*** sceneLeaves;
	int totalEnemies;

	XMFLOAT4* grassPositions;
	XMFLOAT4* timer;
	GUI* vida;
	GUI* antholeHealth[2];
	GUI* leafCurrency[2];
	GUI* greenRightArrow;
	Text* text;
	Text* titleText[2];

	float izqder;
	float arriaba;
	float vel;
	float* velDir;
	bool breakpoint;
	vector2 uv1[34];
	vector2 uv2[34];
	vector2 uv3[34];
	vector2 uv4[34];

	XACTINDEX cueIndex;
	CXACT3Util m_XACT3;

	bool won;
	bool lost;

	enum assetArray {
		asset,
		assetModel = 0,
		active = 0,
		position,
		type = 1,
		radius = 1,
		scale,
		rotation,
		collision,
		health,
		picked = 5
	};

	enum Positions {
		x,
		y,
		z,
		width,
		height
	};

	enum Collectibles {
		notPicked,
		pickedUp
	};

	enum Models {
		ant,
		anthole,
		house,
		rockPillar,
		rock,
		crystal,
		tree,
		leaf,
		sticks,
		bottle,
		cap,
		worm,
		spider,
		mantis
	};

	struct ShopItem {
		string name = "";
		GUI* icon = NULL;
		int price = 0;
		int model = 0;
	};

	ShopItem* shopList;
	int totalItemsInShop;

    DXRR(HWND hWnd, int Ancho, int Alto)
	{
		breakpoint = false;
		frameBillboard = 0;
		ancho = Ancho;
		alto = Alto;
		driverType = D3D_DRIVER_TYPE_NULL;
		featureLevel = D3D_FEATURE_LEVEL_11_0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
		velDir = 0;
		IniciaD3D(hWnd);
		izqder = 0;
		arriaba = 0;
		billCargaFuego();

		won = false;
		lost = false;

		timer = new XMFLOAT4;
		timer->x = 0.0f;
		timer->y = 0.0f;
		timer->z = 0.0f;
		timer->w = 0.0f;

		globalTimer = 0;
		realTime = 0;

		totalAssets = 14;

		sceneAssets = new ModeloRR** [totalAssets];
		for (int i = 0; i < totalAssets; i++) {
			sceneAssets[i] = new ModeloRR * [3] { NULL };
		}

		terreno = new TerrenoRR(2000, 2000, d3dDevice, d3dContext);
		water = new Water(830, 830, d3dDevice, d3dContext);
		skydome = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Assets/Skydomes/clear.jpg", L"Assets/Skydomes/night.png");
		
		sceneAssets[ant][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/AntModel_Rigged_Smooth.obj", L"Assets/Textures/AntModel_Rigged_Smooth.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		sceneAssets[anthole][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hormiguero.obj", L"Assets/Textures/Hormiguero.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		if (!QUICKLOAD) {
			sceneAssets[house][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Old_stone_house.obj", L"Assets/Textures/Old_stone_house_BaseColor.png", L"Assets/Textures/NoSpecular.png", 20, 0);
			//sceneAssets[rockPillar][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/pilarRoca1.obj", L"Assets/Textures/pilarRoca1_Color.png", L"Assets/Textures/NoSpecular.png", 65, 6);
			sceneAssets[rock][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/ballRock.obj", L"Assets/Textures/ballRock.png", L"Assets/Textures/NoSpecular.png", 5, -5);
			sceneAssets[crystal][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Cristal1.obj", L"Assets/Textures/Cristal.png", L"Assets/Textures/NoSpecular.png", -14, -7);
			sceneAssets[crystal][1] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Cristal2.obj", L"Assets/Textures/Cristal.png", L"Assets/Textures/NoSpecular.png", 4, -17);
			sceneAssets[crystal][2] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Cristal3.obj", L"Assets/Textures/Cristal.png", L"Assets/Textures/NoSpecular.png", 14, 8);
			sceneAssets[tree][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Tree1.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -24.0f, -28.0f);
			sceneAssets[leaf][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hoja1.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", -8, 73);
			sceneAssets[leaf][1] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hoja2.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", -29, 54);
			sceneAssets[leaf][2] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hoja3.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", -36, 74);
			sceneAssets[sticks][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Stick1.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -14, 62);
			sceneAssets[sticks][1] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Stick2.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -18, 88);
			sceneAssets[sticks][2] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Stick3.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -39, 70);
			sceneAssets[bottle][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Botella.obj", L"Assets/Textures/Botella.png", L"Assets/Textures/NoSpecular.png", 10, 10);
			//sceneAssets[cap][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/cap.obj", L"Assets/Textures/cap.png", L"Assets/Textures/NoSpecular.png", -24.0f, -28.0f);
		}
		sceneAssets[worm][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/wormVehicle.obj", L"Assets/Textures/Hormiguero.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		sceneAssets[spider][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/spider.obj", L"Assets/Textures/AntModel_Rigged_Smooth.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		sceneAssets[mantis][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/mantis.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", 0, 0);

		//billboard = new BillboardRR(L"Assets/Billboards/fuego-anim.png",L"Assets/Billboards/fuego-anim-normal.png", d3dDevice, d3dContext, 5);
		frameSmoke = 15;
		smoke = new BillboardRR(L"Assets/Billboards/smoke-anim.png",L"Assets/Billboards/smoke-anim-normal.png", d3dDevice, d3dContext, 5);
		billCargaHumo();
		antSalesman = new BillboardRR(L"Assets/Billboards/antSalesman.png",L"Assets/Billboards/antSalesman-normal.png", d3dDevice, d3dContext, 5);
		rustySign = new BillboardRR(L"Assets/Billboards/rustySign.png",L"Assets/Billboards/rustySign-normal.png", d3dDevice, d3dContext, 5);
		mountains = new BillboardRR(L"Assets/Billboards/mountains.png",L"Assets/Billboards/mountains-normal.png", d3dDevice, d3dContext, 10);

		grass = new BillboardRR(L"Assets/Billboards/grass.png",L"Assets/Billboards/grass-normal.png", d3dDevice, d3dContext, 5);
		totalGrass = 1000;
		grassPositions = new XMFLOAT4[totalGrass];
		int tries = 0;
		float tempX = 0;
		float tempZ = 0;
		for (int i = 0; i < totalGrass; i++) {
			tempX = generateRandom();
			tempZ = generateRandom();
			if (tempX > -60 && tempX < 60)
			{
				while (tempZ > -60 && tempZ < 60) {
					tempZ = generateRandom();
					tries++;
					if (tries > 9999)
						break;
				}
			}
			grassPositions[i] = XMFLOAT4(tempX, terreno->Superficie(tempX, tempZ), tempZ, rand() % 2);
		}
		//model = new ModeloRR(d3dDevice, d3dContext, "Assets/Cofre/Cofre.obj", L"Assets/Cofre/Cofre-color.png", L"Assets/Cofre/Cofre-spec.png", 0, 0);
		
		player = new Player(D3DXVECTOR3(0, 80, 0), Ancho, Alto, sceneAssets[ant], 1, 1);
		player->SetPosition(2, terreno->Superficie(player->GetPosition().x, player->GetPosition().z));

		sceneAnthole = new float* [6];
		for (int i = 0; i < 6; i++) {
			sceneAnthole[i] = new float[3] {0.0f};
		}

		sceneAnthole[asset][assetModel] = anthole;
		sceneAnthole[position][x] = 0;
		sceneAnthole[position][y] -= 1.0f;
		sceneAnthole[position][z] = 0;
		sceneAnthole[scale][0] = 4.0f;
		sceneAnthole[collision][active] = 0.0f;
		sceneAnthole[collision][radius] = 40.0f;
		sceneAnthole[health][0] = 100.0f;

		totalEnemies = 10;

		spiderEnemies = new Enemy*[totalEnemies];
		totalTargets = 2;
		sceneTargets = new float** [totalTargets];
		sceneTargets[0] = player->getPlayerInfo();
		sceneTargets[1] = getAntholeInfo();

		for (int i = 0; i < totalEnemies; i++) {
			spiderEnemies[i] = new Enemy(D3DXVECTOR3(rand() % 2 == 0 ? rand() % 701 - 800 : rand() % 701 + 100, 80, rand() % 2 == 0 ? rand() % 701 - 800 : rand() % 701 + 100), sceneTargets, totalTargets, &m_XACT3, sceneAssets[spider], 1, 1);
			spiderEnemies[i]->SetPosition(2, terreno->Superficie(spiderEnemies[i]->GetPosition().x, spiderEnemies[i]->GetPosition().z));
		}

		maxTurrets = 10;
		TurretTypes = 1;
		currentTurrets = 0;

		sceneTurrets = new Turret**[TurretTypes];

		for (int i = 0; i < TurretTypes; i++) {
			sceneTurrets[i] = new Turret* [maxTurrets]{ NULL };
		}

		int count = 0;
		for (int i = 0; i < maxTurrets; i++) {
			sceneTurrets[0][i] = new Turret(D3DXVECTOR3(0, 80, 0), getEnemiesInfo(count), count, &m_XACT3, sceneAssets[mantis], 1, 1);
		}

		totalLeaves = 3000;
		sceneLeaves = new float** [totalLeaves];
		for (int i = 0; i < totalLeaves; i++) {
			sceneLeaves[i] = new float* [6];
			for (int j = 0; j < 6; j++) {
				sceneLeaves[i][j] = new float[3] {0.0f};
			}
			sceneLeaves[i][asset][assetModel] = leaf;
			sceneLeaves[i][asset][type] = rand() % 3;
			sceneLeaves[i][position][x] = generateRandom();
			sceneLeaves[i][position][z] = generateRandom();
			if (sceneLeaves[i][position][x] > -60 && sceneLeaves[i][position][x] < 60)
			{
				while (sceneLeaves[i][position][z] > -60 && sceneLeaves[i][position][z] < 60)
					sceneLeaves[i][position][z] = generateRandom();
			}
			sceneLeaves[i][position][y] = terreno->Superficie(sceneLeaves[i][position][x], sceneLeaves[i][position][z]);
			sceneLeaves[i][scale][0] = 1.0f;
			sceneLeaves[i][rotation][y] = rand() % 361;
			sceneLeaves[i][collision][active] = 0.0f;
			sceneLeaves[i][collision][radius] = 8.0f;
			sceneLeaves[i][picked][0] = pickedUp;
		}
		
		totalTrees = 30;

		totalModels = 13 + totalTrees;

		sceneModels = new float** [totalModels];
		for (int i = 0; i < totalModels; i++) {
			sceneModels[i] = new float* [5];
			for (int j = 0; j < 5; j++) {
				sceneModels[i][j] = new float[3] {0.0f};
			}
		}

		sceneModels[0][asset][assetModel] = ant;
		sceneModels[0][position][x] = 0;
		sceneModels[0][position][z] = 0;
		sceneModels[0][scale][0] = 1.0f;
		sceneModels[0][collision][active] = 0.0f;

		sceneModels[1][asset][assetModel] = anthole;
		sceneModels[1][position][x] = 0;
		sceneModels[1][position][y] -= 1.0f;
		sceneModels[1][position][z] = 0;
		sceneModels[1][scale][0] = 4.0f;
		sceneModels[1][collision][active] = 0.0f;

		sceneModels[2][asset][assetModel] = house;
		sceneModels[2][position][x] = 800;
		sceneModels[2][position][z] = -500;
		sceneModels[2][scale][0] = 1.5f;
		sceneModels[2][collision][active] = 1.0f;
		sceneModels[2][collision][radius] = 1.0f;

		sceneModels[3][asset][assetModel] = rockPillar;
		sceneModels[3][position][x] = 65;
		sceneModels[3][position][z] = 6;
		sceneModels[3][scale][0] = 1.0f;
		sceneModels[3][collision][active] = 1.0f;
		sceneModels[3][collision][radius] = 1.0f;

		sceneModels[4][asset][assetModel] = rock;
		sceneModels[4][position][x] = 5;
		sceneModels[4][position][z] = -5;
		sceneModels[4][scale][0] = 1.0f;
		sceneModels[4][collision][active] = 1.0f;
		sceneModels[4][collision][radius] = 1.0f;

		sceneModels[5][asset][assetModel] = crystal;
		sceneModels[5][position][x] = -14;
		sceneModels[5][position][z] = -7;
		sceneModels[5][scale][0] = 1.0f;
		sceneModels[5][collision][active] = 1.0f;
		sceneModels[5][collision][radius] = 1.0f;
		sceneModels[6][asset][assetModel] = crystal;
		sceneModels[6][asset][type] = 1;
		sceneModels[6][position][x] = 4;
		sceneModels[6][position][z] = -17;
		sceneModels[6][scale][0] = 1.0f;
		sceneModels[6][collision][active] = 1.0f;
		sceneModels[6][collision][radius] = 1.0f;
		sceneModels[7][asset][assetModel] = crystal;
		sceneModels[7][asset][type] = 2;
		sceneModels[7][position][x] = 14;
		sceneModels[7][position][z] = 8;
		sceneModels[7][scale][0] = 1.0f;
		sceneModels[7][collision][active] = 1.0f;
		sceneModels[7][collision][radius] = 1.0f;

		sceneModels[8][asset][assetModel] = sticks;
		sceneModels[8][position][x] = -14;
		sceneModels[8][position][z] = 62;
		sceneModels[8][scale][0] = 1.0f;
		sceneModels[8][collision][active] = 1.0f;
		sceneModels[8][collision][radius] = 1.0f;
		sceneModels[9][asset][assetModel] = sticks;
		sceneModels[9][asset][type] = 1;
		sceneModels[9][position][x] = -18;
		sceneModels[9][position][z] = 88;
		sceneModels[9][scale][0] = 1.0f;
		sceneModels[9][collision][active] = 1.0f;
		sceneModels[9][collision][radius] = 1.0f;
		sceneModels[10][asset][assetModel] = sticks;
		sceneModels[10][asset][type] = 2;
		sceneModels[10][position][x] = -39;
		sceneModels[10][position][z] = 70;
		sceneModels[10][scale][0] = 1.0f;
		sceneModels[10][collision][active] = 1.0f;
		sceneModels[10][collision][radius] = 1.0f;

		sceneModels[11][asset][assetModel] = bottle;
		sceneModels[11][position][x] = 10.0f;
		sceneModels[11][position][y] -= 5.0f;
		sceneModels[11][position][z] = -60.0f;
		sceneModels[11][rotation][x] = 45.0f;
		sceneModels[11][scale][0] = 4.0f;
		sceneModels[11][collision][active] = 1.0f;
		sceneModels[11][collision][radius] = 1.0f;

		sceneModels[12][asset][assetModel] = cap;
		sceneModels[12][position][x] = -24.0f;
		sceneModels[12][position][z] = -28.0f;
		sceneModels[12][scale][0] = 0.1f;
		sceneModels[12][collision][active] = 1.0f;
		sceneModels[12][collision][radius] = 1.0f;

		for (int i = 12; i < totalTrees + 12; i++) {
			sceneModels[i][asset][assetModel] = tree;
			sceneModels[i][position][x] = generateRandom();
			sceneModels[i][position][z] = generateRandom();
			sceneModels[i][scale][0] = (rand() % 3) + 1;
			sceneModels[i][collision][active] = 1.0f;
			sceneModels[i][collision][radius] = sceneModels[i][scale][0] * 10.0f;
			sceneModels[i][rotation][y] = rand() % 361;
		}

		for (int i = 1; i < totalModels; i++) {
			sceneModels[i][position][y] = terreno->Superficie(sceneModels[i][position][x], sceneModels[i][position][z]);
		}

		//scale 5
		sceneVehicle = new float* [5];
		for (int i = 0; i < 5; i++) {
			sceneVehicle[i] = new float[3] {0.0f};
		}

		sceneVehicle[asset][assetModel] = worm;
		sceneVehicle[position][x] = 0.0f;
		sceneVehicle[position][y] = terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]);
		sceneVehicle[position][z] = 0.0f;
		sceneVehicle[scale][0] = 5.0f;
		sceneVehicle[collision][active] = 0.0f;
		sceneVehicle[collision][radius] = 5.0f;

		totalWalls = 5;
		sceneWalls = new float* [totalWalls];
		for (int i = 0; i < totalWalls; i++) {
			sceneWalls[i] = new float[5] {0.0f};
		}

		sceneWalls[0][x] = 2.0f;
		sceneWalls[0][y] = 17.0f;
		sceneWalls[0][width] = 28.0f;
		sceneWalls[0][height] = 13.0f;

		sceneWalls[1][x] = 17.0f;
		sceneWalls[1][y] = -30.0f;
		sceneWalls[1][width] = 13.0f;
		sceneWalls[1][height] = 57.0f;

		sceneWalls[2][x] = -30.0f;
		sceneWalls[2][y] = -30.0f;
		sceneWalls[2][width] = 57.0f;
		sceneWalls[2][height] = 13.0f;

		sceneWalls[3][x] = -30.0f;
		sceneWalls[3][y] = 17.0f;
		sceneWalls[3][width] = 28.0f;
		sceneWalls[3][height] = 13.0f;

		sceneWalls[4][x] = -30.0f;
		sceneWalls[4][y] = -30.0f;
		sceneWalls[4][width] = 13.0f;
		sceneWalls[4][height] = 57.0f;

		//sceneWalls[5][x] = 2.0f;
		//sceneWalls[5][y] = 30.0f;
		//sceneWalls[5][width] = 28.0f;
		//sceneWalls[5][height] = 1.0f;

		//sceneWalls[6][x] = 30.0f;
		//sceneWalls[6][y] = -30.0f;
		//sceneWalls[6][width] = 1.0f;
		//sceneWalls[6][height] = 57.0f;

		//sceneWalls[7][x] = -30.0f;
		//sceneWalls[7][y] = -30.0f;
		//sceneWalls[7][width] = 57.0f;
		//sceneWalls[7][height] = 1.0f;

		//sceneWalls[8][x] = -30.0f;
		//sceneWalls[8][y] = 30.0f;
		//sceneWalls[8][width] = 28.0f;
		//sceneWalls[8][height] = 1.0f;

		//sceneWalls[9][x] = -30.0f;
		//sceneWalls[9][y] = -30.0f;
		//sceneWalls[9][width] = 1.0f;
		//sceneWalls[9][height] = 57.0f;

		vida = new GUI(d3dDevice, d3dContext, 0.3f, 0.3f, L"Assets/GUI/ant_health.png");
		greenRightArrow = new GUI(d3dDevice, d3dContext, 0.1f, 0.2f, L"Assets/GUI/greenRightArrow.png");
		leafCurrency[0] = new GUI(d3dDevice, d3dContext, 0.3f, 0.22f, L"Assets/GUI/leaf.png");
		leafCurrency[1] = new GUI(d3dDevice, d3dContext, 0.15f, 0.15f, L"Assets/GUI/leaf.png");
		antholeHealth[0] = new GUI(d3dDevice, d3dContext, 0.1f, 0.05f, L"Assets/GUI/anthole_health.png");
		antholeHealth[1] = new GUI(d3dDevice, d3dContext, 0.1f, 0.05f, L"Assets/GUI/anthole_missingHealth.png");
		text = new Text(d3dDevice, d3dContext, 3.6f, 1.2f, L"Assets/GUI/font.png", XMFLOAT4(0.7f, 0.7f, 0.7f, 0.0f));
		titleText[0] = new Text(d3dDevice, d3dContext, 8.0f, 6.0f, L"Assets/GUI/font.png", XMFLOAT4(0.13f, 0.55f, 0.13f, 0.0f));
		titleText[1] = new Text(d3dDevice, d3dContext, 8.0f, 6.0f, L"Assets/GUI/font.png", XMFLOAT4(0.86f, 0.08f, 0.24f, 0.0f));

		totalItemsInShop = 1;
		shopList = new ShopItem[totalItemsInShop];

		shopList[0].name = "Mantis";
		shopList[0].model = mantis;
		shopList[0].price = 1;
		shopList[0].icon = new GUI(d3dDevice, d3dContext, 0.2f, 0.2f, L"Assets/GUI/mantis.png");;

	}

	~DXRR()
	{
		LiberaD3D();
		m_XACT3.Terminate();
	}
	
	bool IniciaD3D(HWND hWnd)
	{
		this->hInstance = hInstance;
		this->hWnd = hWnd;

		//obtiene el ancho y alto de la ventana donde se dibuja
		RECT dimensions;
		GetClientRect(hWnd, &dimensions);
		unsigned int width = dimensions.right - dimensions.left;
		unsigned int heigth = dimensions.bottom - dimensions.top;

		//Las formas en como la pc puede ejecutar el DX11, la mas rapida es D3D_DRIVER_TYPE_HARDWARE pero solo se puede usar cuando lo soporte el hardware
		//otra opcion es D3D_DRIVER_TYPE_WARP que emula el DX11 en los equipos que no lo soportan
		//la opcion menos recomendada es D3D_DRIVER_TYPE_SOFTWARE, es la mas lenta y solo es util cuando se libera una version de DX que no sea soportada por hardware
		D3D_DRIVER_TYPE driverTypes[]=
		{
			D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
		};
		unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

		//La version de DX que utilizara, en este caso el DX11
		D3D_FEATURE_LEVEL featureLevels[]=
		{
			D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
		};
		unsigned int totalFeaturesLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = heigth;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		HRESULT result;
		unsigned int driver = 0, creationFlags = 0;
		for(driver = 0; driver<totalDriverTypes; driver++)
		{
			result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
				creationFlags, featureLevels, totalFeaturesLevels, 
				D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
				&d3dDevice, &featureLevel, &d3dContext);

			if(SUCCEEDED(result))
			{
				driverType = driverTypes[driver];
				break;
			}
		}

		if(FAILED(result))
		{

			//Error al crear el Direct3D device
			return false;
		}
		
		ID3D11Texture2D* backBufferTexture;
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
		if(FAILED(result))
		{
			//"Error al crear el swapChainBuffer
			return false;
		}

		result = d3dDevice->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget);
		if(backBufferTexture)
			backBufferTexture->Release();

		if(FAILED(result))
		{
			//Error al crear el renderTargetView
			return false;
		}

		D3D11_VIEWPORT viewport;
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)heigth;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		d3dContext->RSSetViewports(1, &viewport);

		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width;
		depthTexDesc.Height = heigth;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		
		result = d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &depthTexture);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear la DepthTexture", MB_OK);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		result = d3dDevice->CreateDepthStencilView(depthTexture, &descDSV, &depthStencilView);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el depth stencil target view", MB_OK);
			return false;
		}

		d3dContext->OMSetRenderTargets(1, &backBufferTarget, depthStencilView);
			
		velDir = new float[3] { 0.0f };

		if (!m_XACT3.Initialize()) {
			MessageBox(hWnd, L"Error initializing XACT", L"ERROR", MB_OK | MB_ICONERROR);
			return false;
		}

		if (!m_XACT3.LoadWaveBank(L"Assets\\Audio\\WaveBank.xwb")) {
			MessageBox(hWnd, L"Error loading Wave Bank", L"ERROR", MB_OK | MB_ICONERROR);
			return false;
		}

		if (!m_XACT3.LoadSoundBank(L"Assets\\Audio\\SoundBank.xsb")) {
			MessageBox(hWnd, L"Error loading Sound Bank", L"ERROR", MB_OK | MB_ICONERROR);
			return false;
		}

		// Cargar los indeces de los Cues
		cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("Fondo");
		m_XACT3.m_pSoundBank->Play(cueIndex, 0, 0, 0);

		return true;
		
	}

	void LiberaD3D(void)
	{
		if(depthTexture)
			depthTexture->Release();
		if(depthStencilView)
			depthStencilView->Release();
		if(backBufferTarget)
			backBufferTarget->Release();
		if(swapChain)
			swapChain->Release();
		if(d3dContext)
			d3dContext->Release();
		if(d3dDevice)
			d3dDevice->Release();

		depthTexture = 0;
		depthStencilView = 0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
		timer = 0;

		if (velDir)
			delete[] velDir;
		velDir = 0;

		for (int i = 0; i < totalAssets; i++) {
			delete[] sceneAssets[i];
		}
		delete[] sceneAssets;

		for (int i = 0; i < totalModels; i++) {
			for (int j = 0; j < 5; j++) {
				delete[] sceneModels[i][j];
			}
			delete[] sceneModels[i];
		}
		delete[] sceneModels;

		for (int i = 0; i < TurretTypes; i++) {
			for (int j = 0; j < maxTurrets; j++) {
				delete[] sceneTurrets[i][j];
			}
			delete[] sceneTurrets[i];
		}
		delete[] sceneTurrets;

		for (int i = 0; i < totalAssets; i++) {
			delete[] sceneAssets[i];
		}
		delete[] sceneAssets;

		for (int i = 0; i < totalLeaves; i++) {
			delete[] sceneLeaves[i];
		}
		delete[] sceneLeaves;

		for (int i = 0; i < totalTargets; i++) {
			delete[] sceneTargets[i];
		}
		delete[] sceneTargets;

		delete[] sceneAnthole;
		for (int i = 0; i < totalWalls; i++) {
			delete[] sceneWalls[i];
		}
		delete[] sceneWalls;

		for (int i = 0; i < 2; i++) {
			delete antholeHealth[i];
		}
		delete[] antholeHealth;

		for (int i = 0; i < 2; i++) {
			delete leafCurrency[i];
		}
		delete[] leafCurrency;

		delete[] shopList;
	}
	
	void Render(void)
	{
		globalTimer++;
		if (!won && !lost && globalTimer % 60 == 0) {
			realTime++;
		}

		if (!won && (*player->getHealth() <= 0 || sceneAnthole[health][0] <= 0))
			lost = true;
		if (realTime >= GAMEDURATION && !lost)
			won = true;
		if (!GAMEENDS) {
			lost = false;
			won = false;
		}

		if (timer->y == 0.0f)
			timer->x += DAYCYCLESPEED;
		else
			timer->x -= DAYCYCLESPEED;

		if (timer->x >= 1.0f)
			timer->y = 1.0f;
		else if (timer->x <= 0.0f)
			timer->y = 0.0f;

		prevPos = new float[3]{ player->GetPosition().x, player->GetPosition().y, player->GetPosition().z };
		
		if( d3dContext == 0 )
			return;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView( backBufferTarget, clearColor );
		d3dContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

		if (player->GetPosition().y > terreno->Superficie(player->GetPosition().x, player->GetPosition().z)) {
			player->SetAcceleration(2, GRAVITYFORCE);
		}

		if(!won && !lost)
			player->MovePlayer(vel, velDir, arriaba, izqder, sceneModels, totalModels, sceneWalls, totalWalls);
		else
			player->MovePlayer(0, velDir, 0, 0, sceneModels, totalModels, sceneWalls, totalWalls);

		Camara* playerCamera = player->GetCamera();

		skydome->Update(playerCamera->vista, playerCamera->proyeccion);

		TurnOffDepth();
		skydome->Render(playerCamera->posCam, timer);
		TurnOnDepth();	

		timer->z = realTime > 0 ? realTime / GAMEDURATION : 0;
		terreno->Draw(playerCamera->vista, playerCamera->proyeccion, (float*)timer);
		timer->z = 0.0f;

		player->Draw(timer);
		if (globalTimer % 600 == 0) {
			respawnEnemies(3);
		}
		for (int i = 0; i < totalEnemies; i++) {
			spiderEnemies[i]->possibleTargets = sceneTargets;
			spiderEnemies[i]->Draw(playerCamera->vista, playerCamera->proyeccion, playerCamera->posCam, 1.0f, 5.0f, timer);
			spiderEnemies[i]->SetPosition(2, terreno->Superficie(spiderEnemies[i]->GetPosition().x, spiderEnemies[i]->GetPosition().z));
			if (!won && !lost)
				spiderEnemies[i]->MoveEnemy(0.3f);
		}

		UpdateTurrets(true);

		int a = sceneVehicle[asset][assetModel];
		int t = sceneVehicle[asset][type];
		float s = sceneVehicle[scale][0];
		float* p = sceneVehicle[position];

		if (sceneAssets[a][t]) {
			if (player->isDriving) {
				D3DXVECTOR3 playerRef = player->GetFrontReference2D();
				float rotAngle = playerRef.z > 0 ? atanf(playerRef.x / playerRef.z) - (D3DX_PI / 2) : atanf(playerRef.x / playerRef.z) + (D3DX_PI / 2);
				sceneVehicle[rotation][y] = rotAngle;
			}
			sceneAssets[a][t]->Draw(playerCamera->vista, playerCamera->proyeccion, p, playerCamera->posCam, 1.0f, sceneVehicle[rotation][y], 'Y', s, timer);
		}

		UpdateLeaves();

		if (player->isJumping && player->GetPosition().y < terreno->Superficie(player->GetPosition().x, player->GetPosition().z)) {
			player->isJumping = false;
		}

		if (!player->isJumping) {
			if (player->isDriving) {
				sceneVehicle[position][y] = terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]);
			}
			else {
				player->SetPosition(2, terreno->Superficie(player->GetPosition().x, player->GetPosition().z));
			}
		}

		DrawVehicle();

		DrawStaticModels();

		DrawPlacementModel();

		TurnOnAlphaBlending();
		water->Draw(playerCamera->vista, playerCamera->proyeccion, (float*)timer, new float[4] {(float)globalTimer, 0.0f, 0.0f, 0.0f});
		TurnOffAlphaBlending();

		/*int a2 = sceneModels[16][asset][assetModel];
		float* p2 = sceneModels[16][position];
		float s2 = sceneModels[16][scale][0];
		p2[x] = 28.0f;
		p2[z] = 28.0f;
		sceneModels[16][position][y] = terreno->Superficie(sceneModels[16][position][x], sceneModels[16][position][z]);
		sceneAssets[a2][0]->Draw(playerCamera->vista, playerCamera->proyeccion, p2, playerCamera->posCam, 1.0f, 0, 'N', s2, timer);
		p2[x] = 28.0f;
		p2[z] = -28.0f;
		sceneModels[16][position][y] = terreno->Superficie(sceneModels[16][position][x], sceneModels[16][position][z]);
		sceneAssets[a2][0]->Draw(playerCamera->vista, playerCamera->proyeccion, p2, playerCamera->posCam, 1.0f, 0, 'N', s2, timer);
		p2[x] = -28.0f;
		p2[z] = -28.0f;
		sceneModels[16][position][y] = terreno->Superficie(sceneModels[16][position][x], sceneModels[16][position][z]);
		sceneAssets[a2][0]->Draw(playerCamera->vista, playerCamera->proyeccion, p2, playerCamera->posCam, 1.0f, 0, 'N', s2, timer);
		p2[x] = -28.0f;
		p2[z] = 28.0f;
		sceneModels[16][position][y] = terreno->Superficie(sceneModels[16][position][x], sceneModels[16][position][z]);
		sceneAssets[a2][0]->Draw(playerCamera->vista, playerCamera->proyeccion, p2, playerCamera->posCam, 1.0f, 0, 'N', s2, timer);
		p2[x] = -3.0f;
		p2[z] = 28.0f;
		sceneModels[16][position][y] = terreno->Superficie(sceneModels[16][position][x], sceneModels[16][position][z]);
		sceneAssets[a2][0]->Draw(playerCamera->vista, playerCamera->proyeccion, p2, playerCamera->posCam, 1.0f, 0, 'N', s2, timer);
		p2[x] = 3.0f;
		p2[z] = 28.0f;
		sceneModels[16][position][y] = terreno->Superficie(sceneModels[16][position][x], sceneModels[16][position][z]);
		sceneAssets[a2][0]->Draw(playerCamera->vista, playerCamera->proyeccion, p2, playerCamera->posCam, 1.0f, 0, 'N', s2, timer);*/

		DrawUI();

		DrawTitleUI();

		swapChain->Present( 1, 0 );
	}

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

	//Activa el alpha blend para dibujar con transparencias
	void TurnOnAlphaBlending()
	{
		float blendFactor[4];
		blendFactor[0] = 0.0f;
		blendFactor[1] = 0.0f;
		blendFactor[2] = 0.0f;
		blendFactor[3] = 0.0f;
		HRESULT result;

		D3D11_BLEND_DESC descABSD;
		ZeroMemory(&descABSD, sizeof(D3D11_BLEND_DESC));
		descABSD.RenderTarget[0].BlendEnable = TRUE;
		descABSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descABSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descABSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		result = d3dDevice->CreateBlendState(&descABSD, &alphaBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(alphaBlendState, blendFactor, 0xffffffff);
	}

	//Regresa al blend normal(solido)
	void TurnOffAlphaBlending()
	{
		D3D11_BLEND_DESC descCBSD;
		ZeroMemory(&descCBSD, sizeof(D3D11_BLEND_DESC));
		descCBSD.RenderTarget[0].BlendEnable = FALSE;
		descCBSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descCBSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descCBSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		HRESULT result;

		result = d3dDevice->CreateBlendState(&descCBSD, &commonBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(commonBlendState, NULL, 0xffffffff);
	}

	void TurnOnDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDSD;
		ZeroMemory(&descDSD, sizeof(descDSD));
		descDSD.DepthEnable = true;
		descDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDSD.StencilEnable=true;
		descDSD.StencilReadMask = 0xFF;
		descDSD.StencilWriteMask = 0xFF;
		descDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDSD, &depthStencilState);
		
		d3dContext->OMSetDepthStencilState(depthStencilState, 1);
	}

	void TurnOffDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDDSD;
		ZeroMemory(&descDDSD, sizeof(descDDSD));
		descDDSD.DepthEnable = false;
		descDDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDDSD.StencilEnable=true;
		descDDSD.StencilReadMask = 0xFF;
		descDDSD.StencilWriteMask = 0xFF;
		descDDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDDSD, &depthStencilDisabledState);
		d3dContext->OMSetDepthStencilState(depthStencilDisabledState, 1);
	}

	void billCargaHumo()
	{
		uv1[0].u = .125;
		uv2[0].u = .125;
		uv3[0].u = 0;
		uv4[0].u = 0;

		uv1[0].v = .20;
		uv2[0].v = 0;
		uv3[0].v = 0;
		uv4[0].v = .20;


		for (int j = 0; j < 8; j++) {
			uv1[j].u = uv1[0].u + (j * .125);
			uv2[j].u = uv2[0].u + (j * .125);
			uv3[j].u = uv3[0].u + (j * .125);
			uv4[j].u = uv4[0].u + (j * .125);

			uv1[j].v = .20;
			uv2[j].v = 0;
			uv3[j].v = 0;
			uv4[j].v = .20;
		}
		for (int j = 0; j < 8; j++) {
			uv1[j + 8].u = uv1[0].u + (j * .125);
			uv2[j + 8].u = uv2[0].u + (j * .125);
			uv3[j + 8].u = uv3[0].u + (j * .125);
			uv4[j + 8].u = uv4[0].u + (j * .125);

			uv1[j + 8].v = .4;
			uv2[j + 8].v = .2;
			uv3[j + 8].v = .2;
			uv4[j + 8].v = .4;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 16].u = uv1[0].u + (j * .125);
			uv2[j + 16].u = uv2[0].u + (j * .125);
			uv3[j + 16].u = uv3[0].u + (j * .125);
			uv4[j + 16].u = uv4[0].u + (j * .125);

			uv1[j + 16].v = .6;
			uv2[j + 16].v = .4;
			uv3[j + 16].v = .4;
			uv4[j + 16].v = .6;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 24].u = uv1[0].u + (j * .125);
			uv2[j + 24].u = uv2[0].u + (j * .125);
			uv3[j + 24].u = uv3[0].u + (j * .125);
			uv4[j + 24].u = uv4[0].u + (j * .125);

			uv1[j + 24].v = .8;
			uv2[j + 24].v = .6;
			uv3[j + 24].v = .6;
			uv4[j + 24].v = .8;
		}

		for (int j = 0; j < 2; j++) {
			uv1[j + 32].u = uv1[0].u + (j * .125);
			uv2[j + 32].u = uv2[0].u + (j * .125);
			uv3[j + 32].u = uv3[0].u + (j * .125);
			uv4[j + 32].u = uv4[0].u + (j * .125);

			uv1[j + 32].v = 1;
			uv2[j + 32].v = .8;
			uv3[j + 32].v = .8;
			uv4[j + 32].v = 1;
		}
	}
	
	float** getAntholeInfo()
	{
		float** antholeInfo = new float* [3];
		antholeInfo[0] = &sceneAnthole[health][0];
		antholeInfo[1] = sceneAnthole[position];
		antholeInfo[2] = &sceneAnthole[collision][radius];
		
		return antholeInfo;
	}

	void billCargaFuego()
	{
		uv1[0].u = .125;
		uv2[0].u = .125;
		uv3[0].u = 0;
		uv4[0].u = 0;

		uv1[0].v = .25;
		uv2[0].v = 0;
		uv3[0].v = 0;
		uv4[0].v = .25;


		for (int j = 0; j < 8; j++) {
			uv1[j].u = uv1[0].u + (j * .125);
			uv2[j].u = uv2[0].u + (j * .125);
			uv3[j].u = uv3[0].u + (j * .125);
			uv4[j].u = uv4[0].u + (j * .125);

			uv1[j].v = .25;
			uv2[j].v = 0;
			uv3[j].v = 0;
			uv4[j].v = .25;
		}
		for (int j = 0; j < 8; j++) {
			uv1[j + 8].u = uv1[0].u + (j * .125);
			uv2[j + 8].u = uv2[0].u + (j * .125);
			uv3[j + 8].u = uv3[0].u + (j * .125);
			uv4[j + 8].u = uv4[0].u + (j * .125);

			uv1[j + 8].v = .5;
			uv2[j + 8].v = .25;
			uv3[j + 8].v = .25;
			uv4[j + 8].v = .5;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 16].u = uv1[0].u + (j * .125);
			uv2[j + 16].u = uv2[0].u + (j * .125);
			uv3[j + 16].u = uv3[0].u + (j * .125);
			uv4[j + 16].u = uv4[0].u + (j * .125);

			uv1[j + 16].v = .75;
			uv2[j + 16].v = .5;
			uv3[j + 16].v = .5;
			uv4[j + 16].v = .75;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 24].u = uv1[0].u + (j * .125);
			uv2[j + 24].u = uv2[0].u + (j * .125);
			uv3[j + 24].u = uv3[0].u + (j * .125);
			uv4[j + 24].u = uv4[0].u + (j * .125);

			uv1[j + 24].v = 1;
			uv2[j + 24].v = .75;
			uv3[j + 24].v = .75;
			uv4[j + 24].v = 1;
		}
	}

	void DamageEnemies(Enemy** Enemies) {

		for (int i = 0; i < totalEnemies; i++) {
			if (Enemies[i]->isEnemyAlive() && isPointInsideSphere(new float[2] { player->GetPosition().x, player->GetPosition().z}, new float[3] { Enemies[i]->GetPosition().x, Enemies[i]->GetPosition().z, Enemies[i]->getRadius()})) {
				*Enemies[i]->getHealth() -= DAMAGE;
				if (*Enemies[i]->getHealth() <= 0)
					Enemies[i]->killEnemy();
			}
		}

	}

	void respawnLeaves() {

		for (int i = 0; i < totalLeaves; i++) {
			if (sceneLeaves[i][picked][0] == pickedUp) {
				sceneLeaves[i][picked][0] = notPicked;
				sceneLeaves[i][position][x] = generateRandom();
				sceneLeaves[i][position][z] = generateRandom();
				if (sceneLeaves[i][position][x] > -60 && sceneLeaves[i][position][x] < 60)
				{
					while (sceneLeaves[i][position][z] > -60 && sceneLeaves[i][position][z] < 60)
						sceneLeaves[i][position][z] = generateRandom();
				}
				sceneLeaves[i][position][y] = terreno->Superficie(sceneLeaves[i][position][x], sceneLeaves[i][position][z]);
			}
		}

	}
	
	void respawnLeaves(int cant) {

		int numberSpawned = 0;
		int tries = 0;

		for (int i = 0; i < totalLeaves; i++) {
			if (sceneLeaves[i][picked][0] == pickedUp) {
				sceneLeaves[i][picked][0] = notPicked;
				sceneLeaves[i][position][x] = generateRandom();
				sceneLeaves[i][position][z] = generateRandom();
				if (sceneLeaves[i][position][x] > -60 && sceneLeaves[i][position][x] < 60)
				{
					while (sceneLeaves[i][position][z] > -60 && sceneLeaves[i][position][z] < 60)
						sceneLeaves[i][position][z] = generateRandom();
				}
				sceneLeaves[i][position][y] = terreno->Superficie(sceneLeaves[i][position][x], sceneLeaves[i][position][z]);
				numberSpawned++;
				if (numberSpawned >= cant)
					break;
			}
		}
	}

	void respawnEnemies(int cant) {

		int numberSpawned = 0;
		int tries = 0;

		for (int i = 0; i < totalEnemies; i++) {
			if (!spiderEnemies[i]->isEnemyAlive()) {
				spiderEnemies[i]->reviveEnemy();
				float tempX = generateRandom();
				float tempZ = generateRandom();
				if (tempX > -200 && tempX < 200)
				{
					while (tempZ > -200 && tempZ < 200) {
						tempZ = generateRandom();
						tries++;
						if (tries > 9999)
							break;
					}
				}
				spiderEnemies[i]->SetPosition(1, tempX);
				spiderEnemies[i]->SetPosition(3, tempZ);
				numberSpawned++;
				if (numberSpawned >= cant)
					break;
			}
		}
	}

	void pickUpLeaves() {

		for (int i = 0; i < totalLeaves; i++) {
			if (sceneLeaves[i][picked][0] == notPicked && isPointInsideSphere(new float[2] { player->GetPosition().x, player->GetPosition().z}, new float[3] { sceneLeaves[i][position][x], sceneLeaves[i][position][z], sceneLeaves[i][collision][radius]})) {
				sceneLeaves[i][picked][0] = pickedUp;
				player->cantLeaves++;
			}
		}

	}

	int generateRandom()
	{
		int value;
		value = rand() % 1601 - 800;
		return value;
	}

	bool placeTurret() {
		bool placed = false;
		int placingId = player->isPlacing[player->placingId];
		int count = 0;
		if (player->isPlacing[player->placingActive] == 1 && player->removeItemFromInventory(placingId)) {
			for (int i = 0; i < maxTurrets; i++) {
				if (!sceneTurrets[placingId][i]->isTurretAlive()) {
					player->isPlacing[player->placingX] += player->GetFrontReference2D().x * 10.0f;
					player->isPlacing[player->placingZ] += player->GetFrontReference2D().z * 10.0f;
					player->isPlacing[player->placingY] = terreno->Superficie(player->isPlacing[player->placingX], player->isPlacing[player->placingZ]);
					sceneTurrets[placingId][i]->reviveTurret();
					sceneTurrets[placingId][i]->SetPosition(player->GetPosition() + player->GetFrontReference2D() * 10.0f);
					player->isPlacing[player->placingActive] = 0;
					currentTurrets++;
					updatePossibleTargets();
					placed = true;
					break;
				}
			}
		}
		return placed;
	}

	void UpdateTurrets(bool draw = false) {
		int count = 0;
		for (int i = 0; i < TurretTypes; i++) {
			for (int j = 0; j < maxTurrets; j++) {
				if (sceneTurrets[i][j]->isTurretAlive()) {
					if (draw) {
						sceneTurrets[i][j]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam, 1.0f, 1.0f, timer);
					}
					if (!sceneTurrets[i][j]->Update(player->GetPosition(), getEnemiesInfo(count), count)) {
						currentTurrets--;
					}
				}
			}
		}
	}

	void DrawStaticModels() {

		for (int i = 1; i < totalModels; i++) {

			int a = sceneModels[i][asset][assetModel];
			int t = sceneModels[i][asset][type];

			if (sceneAssets[a][t]) {
				float s = sceneModels[i][scale][0];
				float* p = sceneModels[i][position];

				if (sceneModels[i][rotation][x] == 0 && sceneModels[i][rotation][y] == 0 && sceneModels[i][rotation][z] == 0)
					sceneAssets[a][t]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, p, player->GetCamera()->posCam, 1.0f, 0, 'N', s, timer);
				else {
					if (sceneModels[i][rotation][x] != 0)
						sceneAssets[a][t]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, p, player->GetCamera()->posCam, 1.0f, sceneModels[i][rotation][x], 'X', s, timer);
					else if (sceneModels[i][rotation][y] != 0)
						sceneAssets[a][t]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, p, player->GetCamera()->posCam, 1.0f, sceneModels[i][rotation][y], 'Y', s, timer);
					else if (sceneModels[i][rotation][z] != 0)
						sceneAssets[a][t]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, p, player->GetCamera()->posCam, 1.0f, sceneModels[i][rotation][z], 'Z', s, timer);
				}
			}
			else
			{
				sceneModels[i][collision][active] = 0.0f;
			}
		}

	}

	void DrawPlacementModel() {

		if (player->isPlacing[player->placingActive] == 1) {
			int placingModel = player->isPlacing[player->placingModel];
			if (sceneAssets[placingModel][0]) {
				D3DXVECTOR3 playerRef = player->GetFrontReference2D();
				float rotAngle = playerRef.z > 0 ? atanf(playerRef.x / playerRef.z) : atanf(playerRef.x / playerRef.z) - D3DX_PI;
				float s = 1.0f;
				player->isPlacing[player->placingX] = player->GetPosition().x;
				player->isPlacing[player->placingY] = player->GetPosition().y;
				player->isPlacing[player->placingZ] = player->GetPosition().z;
				player->isPlacing[player->placingX] += player->GetFrontReference2D().x * 10.0f;
				player->isPlacing[player->placingZ] += player->GetFrontReference2D().z * 10.0f;
				player->isPlacing[player->placingY] = terreno->Superficie(player->isPlacing[player->placingX], player->isPlacing[player->placingZ]);
				timer->w = 1;
				TurnOnAlphaBlending();
				sceneAssets[placingModel][0]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, new float[3] {player->isPlacing[player->placingX], player->isPlacing[player->placingY], player->isPlacing[player->placingZ]}, player->GetCamera()->posCam, 1.0f, rotAngle, 'Y', s, timer);
				TurnOffAlphaBlending();
				timer->w = 0;
			}
		}

	}

	void DrawVehicle() {

		if (!player->isDriving) {
			if (sceneVehicle[position][y] > terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z])) {
				sceneVehicle[position][y]--;
			}
			else if (sceneVehicle[position][y] < terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]))
				sceneVehicle[position][y] = terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]);
		}
		else {
			if (globalTimer % 2 == 0)
				frameSmoke < 33 ? frameSmoke++ : frameSmoke = 15;
			if (!player->isJumping && (this->prevPos[0] != player->GetPosition().x || this->prevPos[2] != player->GetPosition().z)) {
				TurnOnAlphaBlending();
				if (player->getSpeed()[0] > 0) {
					smoke->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam + (player->GetRightReference2D() * -30.0f), sceneVehicle[position][x] + (player->GetFrontReference2D().x * -2.0f) + (player->GetRightReference2D().x * 1.3f), sceneVehicle[position][z] + (player->GetFrontReference2D().z * -2.0f) + (player->GetRightReference2D().z * 1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
					smoke->DrawHFlipped(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam + (player->GetRightReference2D() * 30.0f), sceneVehicle[position][x] + (player->GetFrontReference2D().x * -2.0f) + (player->GetRightReference2D().x * -1.3f), sceneVehicle[position][z] + (player->GetFrontReference2D().z * -2.0f) + (player->GetRightReference2D().z * -1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
				}
				else {
					smoke->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam + (player->GetRightReference2D() * 30.0f), sceneVehicle[position][x] + (player->GetRightReference2D().x * 1.3f), sceneVehicle[position][z] + (player->GetRightReference2D().z * 1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
					smoke->DrawHFlipped(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam + (player->GetRightReference2D() * -30.0f), sceneVehicle[position][x] + (player->GetRightReference2D().x * -1.3f), sceneVehicle[position][z] + (player->GetRightReference2D().z * -1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
				}
				TurnOffAlphaBlending();
			}
		}

	}

	void DrawTitleUI() {
		if (won) {
			TurnOnAlphaBlending();
			titleText[0]->DrawText(-0.5f, 0.1f, "YOU SURVIVED", 0.04f);
			titleText[0]->DrawText(-0.4f, -0.1f, "THE WINTER", 0.04f);
			TurnOffAlphaBlending();
		}
		else if (lost) {
			TurnOnAlphaBlending();
			if (sceneAnthole[health][0] <= 0) {
				titleText[1]->DrawText(-0.45f, 0.1f, "THE ANTHOLE", 0.04f);
				titleText[1]->DrawText(-0.55f, -0.1f, "WAS DESTROYED", 0.04f);
			}
			if (*player->getHealth() <= 0) {
				titleText[1]->DrawText(-0.45f, 0.0f, "YOU PERISHED", 0.04f);
			}
			TurnOffAlphaBlending();
		}
	}

	void DrawUI() {
		for (int i = 0; i < *player->getHealth() / 35.0f; i++) {
			vida->Draw(0.8f, 0.8f - (i * 0.03f));
		}

		for (int i = 0; i < getAntholeInfo()[0][0]; i++) {
			antholeHealth[0]->Draw(-0.5f + (i * 0.01f), 0.7f);
		}
		for (int i = 0; i < 100; i++) {
			antholeHealth[1]->Draw(-0.5f + (i * 0.01f), 0.7f);
		}


		TurnOnAlphaBlending();
		if (isPointInsideSphere(new float[2] {player->GetPosition().x, player->GetPosition().z}, new float[3] { 0.0f, -7.0f, 10.0f})) {
			antSalesman->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam, 0.0f, -7.0f, terreno->Superficie(0.0f, -7.0f) - 1.0f, 2, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
			for (int i = 0; i < totalItemsInShop; i++) {
				leafCurrency[1]->Draw(-0.9f, 0.5f + (i * 0.1f));
				text->DrawText(-0.83f, 0.48f + (i * 0.1f), to_string(shopList[i].price), 0.01f);
				greenRightArrow->Draw(-0.7f, 0.5f);
				shopList[i].icon->Draw(-0.5f, 0.5f + (i * 0.1f));
				//text->DrawText(-0.8f, 0.4f + (i * 0.1f), "Item " + to_string(i) + " price: " + shopList[i].name, 0.01f);
			}
		}
		else {
			antSalesman->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, new float[3] { 0.0f, 0.0f, 0.0f }, 0.0f, -7.0f, terreno->Superficie(0.0f, -7.0f) - 1.0f, 2, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		}

		mountains->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, new float[3] { 0.0f, 0.0f, 0.0f }, 1200.0f, 0.0f, 0.0f, 1500, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		mountains->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, new float[3] { 0.0f, 0.0f, 0.0f }, -1000.0f, 0.0f, 0.0f, 1500, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		mountains->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, new float[3] { 0.0f, 0.0f, 0.0f }, 0.0f, 1000.0f, 0.0f, 1500, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		mountains->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, new float[3] { 0.0f, 0.0f, 0.0f }, 0.0f, -1000.0f, 0.0f, 1500, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		rustySign->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam, 60.0f, 30.0f, 2.0f, 20, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		for (int i = 0; i < totalGrass; i++) {
			if (grassPositions[i].w == 0)
				grass->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam, grassPositions[i].x, grassPositions[i].z, grassPositions[i].y, 5, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
			else
				grass->DrawHFlipped(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam, grassPositions[i].x, grassPositions[i].z, grassPositions[i].y, 5, new vector2{ 0.0f, 1.0f }, new vector2{ 0.0f, 0.0f }, new vector2{ 1.0f, 0.0f }, new vector2{ 1.0f, 1.0f }, 0);
		}
		//grass->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, player->GetCamera()->posCam, 0.0f, 0.0f, 2.0f, 2, new vector2{0.0f, 1.0f}, new vector2{0.0f, 0.0f}, new vector2{1.0f, 0.0f}, new vector2{1.0f, 1.0f}, 0);

		//text->DrawText(-0.1f, -0.9f, "Bruh como que todo se entrega la primera semana", 0.01f);

		text->DrawText(-0.025f, 0.9f, text->Time(realTime), 0.01f);
		text->DrawText(-0.15f, 0.8f, "ANTHOLE HEALTH", 0.01f);
		//text->DrawText(-0.4f, 0.6f, "HEALTH: " + to_string(*player->getHealth()), 0.01f);

		if (!player->isDriving && isPointInsideSphere(new float[2] {player->GetPosition().x, player->GetPosition().z}, new float[3] { sceneVehicle[1][0], sceneVehicle[1][2], sceneVehicle[4][1]}))
			text->DrawText(-0.2f, -0.8f, "Press the LMB to mount", 0.01f);

		int* itemCount = new int[totalItemsInShop] { 0 };
		for (int i = 0; i < player->totalSlotsInInventory; i++) {
			if (player->inventorySlots[i].isOccupied)
				itemCount[player->inventorySlots[i].item.shopId]++;
		}
		for (int i = 0; i < totalItemsInShop; i++) {
			text->DrawText(-0.8f, -0.8f, to_string(itemCount[i]), 0.01f);
			shopList[i].icon->Draw(-0.9f, -0.8f);
		}

		delete[] itemCount;

		/*text->DrawText(-0.6f, 0.7f, "Speed X:" + to_string(player->getSpeed()[0]) + "  Speed Y:" + to_string(player->getSpeed()[1]) + "  Speed Z:" + to_string(player->getSpeed()[2]), 0.01f);
		text->DrawText(-0.1f, 0.6f, "isJumping: " + player->isJumping, 0.01f);
		text->DrawText(-0.6f, 0.5f, "Accel X:" + to_string(player->getAcceleration()[0]) + "  Accel Y:" + to_string(player->getAcceleration()[1]) + "  Accel Z:" + to_string(player->getAcceleration()[2]), 0.01f);
		text->DrawText(-0.05f, 0.4f, "Camera", 0.01f);
		text->DrawText(-0.4f, 0.3f, "X:" + to_string(player->GetCamera()->posCam.x) + "  Y:" + to_string(player->GetCamera()->posCam.y) + "  Z:" + to_string(player->GetCamera()->posCam.z), 0.01f);*/
		//text->DrawText(-0.4f, 0.8f, "X:" + to_string(player->GetPosition().x) + "  Y:" + to_string(player->GetPosition().y) + "  Z:" + to_string(player->GetPosition().z), 0.01f);
		//for (int i = 0; i < totalEnemies; i++) {				
		//	text->DrawText(-0.8f, 0.7f - (i * 0.1f), "ENEMY " + to_string(i) + ": " + to_string(*spiderEnemies[i]->getHealth()), 0.01f);
		//	text->DrawText(-0.8f, 0.6f - (i * 0.1f), "ENEMY " + to_string(i) + " TARGET X: " + to_string(spiderEnemies[i]->getTarget()[1][0]), 0.01f);
		//	text->DrawText(-0.8f, 0.5f - (i * 0.1f), "ENEMY " + to_string(i) + " TARGET Z: " + to_string(spiderEnemies[i]->getTarget()[1][2]), 0.01f);
		//}
		text->DrawText(-0.75f, 0.7f, to_string(player->cantLeaves), 0.01f);
		TurnOffAlphaBlending();

		leafCurrency[0]->Draw(-0.8f, 0.8f);
	}

	float*** getEnemiesInfo(int &count)
	{
		float*** enemyTargets;
		count = 0;

		for (int i = 0; i < totalEnemies; i++) {
			float healthbuffer = *spiderEnemies[i]->getHealth();
			if (*spiderEnemies[i]->getHealth() > 0) {
				count++;
			}
		}

		enemyTargets = new float** [count];
		int tempCount = 0;

		for (int i = 0; i < totalEnemies; i++) {
			if (*spiderEnemies[i]->getHealth() > 0) {
				enemyTargets[tempCount] = spiderEnemies[i]->getEnemyInfo();
				tempCount++;
			}
		}

		return enemyTargets;
	}

	void UpdateLeaves() {

		if (globalTimer % 300 == 0) {
			respawnLeaves(5);
		}
		for (int i = 0; i < totalLeaves; i++) {
			if (sceneLeaves[i][picked][0] == notPicked) {
				int t = sceneLeaves[i][asset][type];
				float s = sceneLeaves[i][scale][0];
				float* p = sceneLeaves[i][position];
				sceneAssets[leaf][t]->Draw(player->GetCamera()->vista, player->GetCamera()->proyeccion, p, player->GetCamera()->posCam, 1.0f, sceneLeaves[i][rotation][y], 'Y', s, timer);
			}
		}
	}

	void updatePossibleTargets() {

		int count = 0;
		for (int i = 0; i < TurretTypes; i++) {
			for (int j = 0; j < maxTurrets; j++) {
				if (sceneTurrets[i][j]->isTurretAlive()) {
					count++;
				}
			}
		}

		totalTargets += count;

		this->sceneTargets = new float** [totalTargets];
		this->sceneTargets[0] = player->getPlayerInfo();
		this->sceneTargets[1] = getAntholeInfo();

		int tempCount = 0;

		for (int i = 0; i < TurretTypes; i++) {
			for (int j = 0; j < maxTurrets; j++) {
				if (sceneTurrets[i][j]->isTurretAlive()) {
					this->sceneTargets[i + j + 2] = sceneTurrets[i][j]->getTurretInfo();
					tempCount++;
					if (tempCount >= count)
						break;
				}
			}
		}

		for (int i = 0; i < totalEnemies; i++) {
			spiderEnemies[i]->possibleTargets = sceneTargets;
			spiderEnemies[i]->selectNewTarget();
		}
		
	}

};
#endif