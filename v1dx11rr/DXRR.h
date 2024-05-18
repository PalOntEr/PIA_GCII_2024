#ifndef _dxrr
#define _dxrr
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include <cmath>
#include "TerrenoRR.h"
#include "Camara.h"
#include "Player.h"
#include "SkyDome.h"
#include "Billboard.h"
#include "ModeloRR.h"
#include "XACT3Util.h"
#include "GUI.h"
#include "Text.h"
#include "Enemy.h"
#define DAYCYCLESPEED 0.0001f/*0.0001f*/
#define GRAVITYFORCE -0.03f
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

	TerrenoRR* terreno;
	SkyDome* skydome;
	BillboardRR* billboard;
	BillboardRR* smoke;
	Player* player;
	Enemy** spiderEnemies;
	ModeloRR* AntModel_Rigged_Smooth = NULL;
	ModeloRR* Anthole = NULL;
	ModeloRR* House = NULL;
	ModeloRR* pilarRoca1 = NULL;
	ModeloRR* BallRock = NULL;
	ModeloRR** Crystals = NULL;
	ModeloRR* Tree1 = NULL;
	ModeloRR** Hojas = NULL;
	ModeloRR** Sticks = NULL;
	ModeloRR* botella = NULL;
	ModeloRR* Cap = NULL;
	ModeloRR* model = NULL;
	
	ModeloRR*** sceneAssets;
	int totalAssets;

	float*** sceneModels;
	int totalModels;
	float*** sceneTargets;
	int totalTargets;
	float** sceneVehicle;
	float** sceneAnthole;
	float** sceneWalls;
	int totalWalls;

	int totalEnemies;

	XMFLOAT4* timer;
	GUI* vida;
	Text* prueba;

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
		health
	};

	enum Positions {
		x,
		y,
		z,
		width,
		height
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
		spider
	};

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

		timer = new XMFLOAT4;
		timer->x = 0.0f;
		timer->y = 0.0f;
		timer->z = 0.0f;
		timer->w = 0.0f;

		globalTimer = 0;

		totalAssets = 13;

		sceneAssets = new ModeloRR** [totalAssets];
		for (int i = 0; i < totalAssets; i++) {
			sceneAssets[i] = new ModeloRR * [3] { NULL };
		}

		totalTargets = 1;

		sceneTargets = new float** [totalTargets];
		//for (int i = 0; i < totalTargets; i++) {
		//	sceneTargets[i] = new float * [3] { NULL };
		//}

		terreno = new TerrenoRR(2000, 2000, d3dDevice, d3dContext);
		skydome = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Assets/Skydomes/clear.jpg", L"Assets/Skydomes/night.png");
		
		sceneAssets[ant][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/AntModel_Rigged_Smooth.obj", L"Assets/Textures/AntModel_Rigged_Smooth.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		sceneAssets[anthole][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hormiguero.obj", L"Assets/Textures/Hormiguero.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		if (!QUICKLOAD) {
			sceneAssets[house][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Old_stone_house.obj", L"Assets/Textures/Old_stone_house_BaseColor.png", L"Assets/Textures/NoSpecular.png", 20, 0);
			sceneAssets[rockPillar][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/pilarRoca1.obj", L"Assets/Textures/pilarRoca1_Color.png", L"Assets/Textures/NoSpecular.png", 65, 6);
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
			sceneAssets[cap][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/cap.obj", L"Assets/Textures/cap.png", L"Assets/Textures/NoSpecular.png", -24.0f, -28.0f);
		}
		sceneAssets[worm][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/wormVehicle.obj", L"Assets/Textures/Hormiguero.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		sceneAssets[spider][0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/spider.obj", L"Assets/Textures/AntModel_Rigged_Smooth.png", L"Assets/Textures/NoSpecular.png", 0, 0);

		//billboard = new BillboardRR(L"Assets/Billboards/fuego-anim.png",L"Assets/Billboards/fuego-anim-normal.png", d3dDevice, d3dContext, 5);
		frameSmoke = 15;
		smoke = new BillboardRR(L"Assets/Billboards/smoke-anim.png",L"Assets/Billboards/smoke-anim-normal.png", d3dDevice, d3dContext, 5);
		billCargaHumo();
		//model = new ModeloRR(d3dDevice, d3dContext, "Assets/Cofre/Cofre.obj", L"Assets/Cofre/Cofre-color.png", L"Assets/Cofre/Cofre-spec.png", 0, 0);
		
		player = new Player(D3DXVECTOR3(0, 80, 0), Ancho, Alto, sceneAssets[ant], 1, 1);
		player->SetPosition(2, terreno->Superficie(player->GetPosition().x, player->GetPosition().z));

		totalEnemies = 10;

		spiderEnemies = new Enemy*[totalEnemies];
		sceneTargets[0] = player->getPlayerInfo();
		for (int i = 0; i < totalEnemies; i++) {
			spiderEnemies[i] = new Enemy(D3DXVECTOR3(rand() % 200, 80, rand() % 200), sceneTargets, totalTargets, &m_XACT3, sceneAssets[spider], 1, 1);
			spiderEnemies[i]->SetPosition(2, terreno->Superficie(spiderEnemies[i]->GetPosition().x, spiderEnemies[i]->GetPosition().z));
		}
		
		totalModels = 17;

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
		sceneModels[2][position][x] = 20;
		sceneModels[2][position][z] = 0;
		sceneModels[2][scale][0] = 1.0f;
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

		sceneModels[8][asset][assetModel] = tree;
		sceneModels[8][position][x] = -24.0f;
		sceneModels[8][position][z] = 60.0f;
		sceneModels[8][scale][0] = 1.0f;
		sceneModels[8][collision][active] = 1.0f;
		sceneModels[8][collision][radius] = 1.0f;

		sceneModels[9][asset][assetModel] = leaf;
		sceneModels[9][position][x] = -8;
		sceneModels[9][position][z] = 73;
		sceneModels[9][scale][0] = 1.0f;
		sceneModels[9][collision][active] = 1.0f;
		sceneModels[9][collision][radius] = 1.0f;
		sceneModels[10][asset][assetModel] = leaf;
		sceneModels[10][asset][type] = 1;
		sceneModels[10][position][x] = -29;
		sceneModels[10][position][z] = 54;
		sceneModels[10][scale][0] = 1.0f;
		sceneModels[10][collision][active] = 1.0f;
		sceneModels[10][collision][radius] = 1.0f;
		sceneModels[11][asset][assetModel] = leaf;
		sceneModels[11][asset][type] = 2;
		sceneModels[11][position][x] = -36;
		sceneModels[11][position][z] = 74;
		sceneModels[11][scale][0] = 1.0f;
		sceneModels[11][collision][active] = 1.0f;
		sceneModels[11][collision][radius] = 1.0f;

		sceneModels[12][asset][assetModel] = sticks;
		sceneModels[12][position][x] = -14;
		sceneModels[12][position][z] = 62;
		sceneModels[12][scale][0] = 1.0f;
		sceneModels[12][collision][active] = 1.0f;
		sceneModels[12][collision][radius] = 1.0f;
		sceneModels[13][asset][assetModel] = sticks;
		sceneModels[13][asset][type] = 1;
		sceneModels[13][position][x] = -18;
		sceneModels[13][position][z] = 88;
		sceneModels[13][scale][0] = 1.0f;
		sceneModels[13][collision][active] = 1.0f;
		sceneModels[13][collision][radius] = 1.0f;
		sceneModels[14][asset][assetModel] = sticks;
		sceneModels[14][asset][type] = 2;
		sceneModels[14][position][x] = -39;
		sceneModels[14][position][z] = 70;
		sceneModels[14][scale][0] = 1.0f;
		sceneModels[14][collision][active] = 1.0f;
		sceneModels[14][collision][radius] = 1.0f;

		sceneModels[15][asset][assetModel] = bottle;
		sceneModels[15][position][x] = 10.0f;
		sceneModels[15][position][y] -= 5.0f;
		sceneModels[15][position][z] = -60.0f;
		sceneModels[15][rotation][x] = 45.0f;
		sceneModels[15][scale][0] = 4.0f;
		sceneModels[15][collision][active] = 1.0f;
		sceneModels[15][collision][radius] = 1.0f;

		sceneModels[16][asset][assetModel] = cap;
		sceneModels[16][position][x] = -24.0f;
		sceneModels[16][position][z] = -28.0f;
		sceneModels[16][scale][0] = 0.1f;
		sceneModels[16][collision][active] = 1.0f;
		sceneModels[16][collision][radius] = 1.0f;

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
		sceneAnthole[collision][radius] = 10.0f;
		sceneAnthole[health][0] = 100.0f;

		totalWalls = 10;
		sceneWalls = new float* [totalWalls];
		for (int i = 0; i < totalWalls; i++) {
			sceneWalls[i] = new float[5] {0.0f};
		}

		sceneWalls[0][x] = 2.0f;
		sceneWalls[0][y] = 17.0f;
		sceneWalls[0][width] = 15.0f;
		sceneWalls[0][height] = 1.0f;

		sceneWalls[1][x] = 17.0f;
		sceneWalls[1][y] = -17.0f;
		sceneWalls[1][width] = 1.0f;
		sceneWalls[1][height] = 34.0f;

		sceneWalls[2][x] = -17.0f;
		sceneWalls[2][y] = -17.0f;
		sceneWalls[2][width] = 34.0f;
		sceneWalls[2][height] = 1.0f;

		sceneWalls[3][x] = -17.0f;
		sceneWalls[3][y] = 17.0f;
		sceneWalls[3][width] = 15.0f;
		sceneWalls[3][height] = 1.0f;

		sceneWalls[4][x] = -17.0f;
		sceneWalls[4][y] = -17.0f;
		sceneWalls[4][width] = 1.0f;
		sceneWalls[4][height] = 34.0f;

		sceneWalls[5][x] = 2.0f;
		sceneWalls[5][y] = 30.0f;
		sceneWalls[5][width] = 28.0f;
		sceneWalls[5][height] = 1.0f;

		sceneWalls[6][x] = 30.0f;
		sceneWalls[6][y] = -30.0f;
		sceneWalls[6][width] = 1.0f;
		sceneWalls[6][height] = 57.0f;

		sceneWalls[7][x] = -30.0f;
		sceneWalls[7][y] = -30.0f;
		sceneWalls[7][width] = 57.0f;
		sceneWalls[7][height] = 1.0f;

		sceneWalls[8][x] = -30.0f;
		sceneWalls[8][y] = 30.0f;
		sceneWalls[8][width] = 28.0f;
		sceneWalls[8][height] = 1.0f;

		sceneWalls[9][x] = -30.0f;
		sceneWalls[9][y] = -30.0f;
		sceneWalls[9][width] = 1.0f;
		sceneWalls[9][height] = 57.0f;


		vida = new GUI(d3dDevice, d3dContext, 0.3f, 0.3f, L"Assets/GUI/ant_health.png");
		prueba = new Text(d3dDevice, d3dContext, 3.6f, 1.2f, L"Assets/GUI/font.png", XMFLOAT4(0.7f, 0.7f, 0.7f, 0.0f));

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
		//m_XACT3.m_pSoundBank->Play(cueIndex, 0, 0, 0);

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

		for (int i = 0; i < totalAssets; i++) {
			delete[] sceneAssets[i];
		}
		delete[] sceneAssets;

		delete[] sceneTargets;
		delete[] sceneAnthole;
		for (int i = 0; i < totalWalls; i++) {
			delete[] sceneWalls[i];
		}
		delete[] sceneWalls;

	}
	
	void Render(void)
	{
		globalTimer++;

		if (timer->y == 0.0f)
			timer->x += DAYCYCLESPEED;
		else
			timer->x -= DAYCYCLESPEED;

		if (timer->x >= 1.0f)
			timer->y = 1.0f;
		else if (timer->x <= 0.0f)
			timer->y = 0.0f;

		float sphere[3] = { 0,0,0 };
		float prevPos[3] = { player->GetPosition().x, player->GetPosition().y, player->GetPosition().z };
		static float angle = 0.0f;
		angle += 0.005;
		if (angle >= 360) angle = 0.0f;
		bool collide = false;
		if( d3dContext == 0 )
			return;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView( backBufferTarget, clearColor );
		d3dContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

		if (player->GetPosition().y > terreno->Superficie(player->GetPosition().x, player->GetPosition().z)) {
			player->SetAcceleration(2, GRAVITYFORCE);
		}

		player->MovePlayer(vel, velDir, arriaba, izqder, sceneModels, totalModels, sceneWalls, totalWalls);

		Camara* playerCamera = player->GetCamera();

		skydome->Update(playerCamera->vista, playerCamera->proyeccion);

		float camPosXZ[2] = { playerCamera->posCam.x, playerCamera->posCam.z };

		TurnOffDepth();
		skydome->Render(playerCamera->posCam, timer);
		TurnOnDepth();	
		terreno->Draw(playerCamera->vista, playerCamera->proyeccion, (float*)timer);

		player->Draw(timer);
		for (int i = 0; i < totalEnemies; i++) {
			spiderEnemies[i]->Draw(playerCamera->vista, playerCamera->proyeccion, playerCamera->posCam, 1.0f, 5.0f, timer);
			spiderEnemies[i]->SetPosition(2, terreno->Superficie(spiderEnemies[i]->GetPosition().x, spiderEnemies[i]->GetPosition().z));
			spiderEnemies[i]->MoveEnemy(0.3f);
		}

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

		if (!player->isDriving) {
			if (sceneVehicle[position][y] > terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z])) {
				sceneVehicle[position][y]--;
			}
			else if (sceneVehicle[position][y] < terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]))
				sceneVehicle[position][y] = terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]);
		}
		else {
			if(globalTimer % 2 == 0)
				frameSmoke < 33 ? frameSmoke++ : frameSmoke = 15;
			if (!player->isJumping && prevPos[0] != player->GetPosition().x && prevPos[2] != player->GetPosition().z) {
				TurnOnAlphaBlending();
				if (player->getSpeed()[0] > 0) {
					smoke->Draw(playerCamera->vista, playerCamera->proyeccion, playerCamera->posCam + (player->GetRightReference2D() * -30.0f), sceneVehicle[position][x] + (player->GetFrontReference2D().x * -2.0f) + (player->GetRightReference2D().x * 1.3f), sceneVehicle[position][z] + (player->GetFrontReference2D().z * -2.0f) + (player->GetRightReference2D().z * 1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
					smoke->DrawHFlipped(playerCamera->vista, playerCamera->proyeccion, playerCamera->posCam + (player->GetRightReference2D() * 30.0f), sceneVehicle[position][x] + (player->GetFrontReference2D().x * -2.0f) + (player->GetRightReference2D().x * -1.3f), sceneVehicle[position][z] + (player->GetFrontReference2D().z * -2.0f) + (player->GetRightReference2D().z * -1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
				}
				else {
					smoke->Draw(playerCamera->vista, playerCamera->proyeccion, playerCamera->posCam + (player->GetRightReference2D() * 30.0f), sceneVehicle[position][x] + (player->GetRightReference2D().x * 1.3f), sceneVehicle[position][z]+ (player->GetRightReference2D().z * 1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
					smoke->DrawHFlipped(playerCamera->vista, playerCamera->proyeccion, playerCamera->posCam + (player->GetRightReference2D() * -30.0f), sceneVehicle[position][x] + (player->GetRightReference2D().x * -1.3f), sceneVehicle[position][z]+ (player->GetRightReference2D().z * -1.3f), terreno->Superficie(sceneVehicle[position][x], sceneVehicle[position][z]), 2, uv1, uv2, uv3, uv4, frameSmoke);
				}
				TurnOffAlphaBlending();
			}
		}

		//TurnOnAlphaBlending();
		//billboard->Draw(playerCamera->vista, playerCamera->proyeccion, player->GetPosition(),-11, -78, 4, 5, uv1, uv2, uv3, uv4, frameBillboard);
		//TurnOffAlphaBlending();

		for (int i = 1; i < totalModels; i++) {

			int a = sceneModels[i][asset][assetModel];
			int t = sceneModels[i][asset][type];
			float s = sceneModels[i][scale][0];
			float* p = sceneModels[i][position];

			if (sceneAssets[a][t]) {
				if (sceneModels[i][rotation][x] == 0 && sceneModels[i][rotation][y] == 0 && sceneModels[i][rotation][z] == 0)
					sceneAssets[a][t]->Draw(playerCamera->vista, playerCamera->proyeccion, p, playerCamera->posCam, 1.0f, 0, 'N', s, timer);
				else {
					if (sceneModels[i][rotation][x] != 0)
						sceneAssets[a][t]->Draw(playerCamera->vista, playerCamera->proyeccion, p, playerCamera->posCam, 1.0f, sceneModels[i][rotation][x], 'X', s, timer);
					else if (sceneModels[i][rotation][y] != 0)
						sceneAssets[a][t]->Draw(playerCamera->vista, playerCamera->proyeccion, p, playerCamera->posCam, 1.0f, sceneModels[i][rotation][y], 'Y', s, timer);
					else if (sceneModels[i][rotation][z] != 0)
						sceneAssets[a][t]->Draw(playerCamera->vista, playerCamera->proyeccion, p, playerCamera->posCam, 1.0f, sceneModels[i][rotation][z], 'Z', s, timer);
				}
			}
		}

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

		for (int i = 0; i < *player->getHealth() / 35.0f; i++) {
			vida->Draw(-0.8f, 0.8f - (i * 0.03f));
		}

		TurnOnAlphaBlending();
			prueba->DrawText(-0.1f, -0.9f, "Bruh como que todo se entrega la primera semana", 0.01f);
			prueba->DrawText(-0.4f, 0.8f, "X:" + to_string(player->GetPosition().x) + "  Y:" + to_string(player->GetPosition().y) + "  Z:" + to_string(player->GetPosition().z), 0.01f);
			/*prueba->DrawText(-0.6f, 0.7f, "Speed X:" + to_string(player->getSpeed()[0]) + "  Speed Y:" + to_string(player->getSpeed()[1]) + "  Speed Z:" + to_string(player->getSpeed()[2]), 0.01f);
			prueba->DrawText(-0.1f, 0.6f, "isJumping: " + player->isJumping, 0.01f);
			prueba->DrawText(-0.6f, 0.5f, "Accel X:" + to_string(player->getAcceleration()[0]) + "  Accel Y:" + to_string(player->getAcceleration()[1]) + "  Accel Z:" + to_string(player->getAcceleration()[2]), 0.01f);
			prueba->DrawText(-0.05f, 0.4f, "Camera", 0.01f);
			prueba->DrawText(-0.4f, 0.3f, "X:" + to_string(playerCamera->posCam.x) + "  Y:" + to_string(playerCamera->posCam.y) + "  Z:" + to_string(playerCamera->posCam.z), 0.01f);*/
			prueba->DrawText(-0.4f, 0.8f, "HEALTH: " + to_string(*player->getHealth()), 0.01f);
			if (!player->isDriving && isPointInsideSphere(new float[2] {player->GetPosition().x, player->GetPosition().z}, new float[3] { sceneVehicle[1][0], sceneVehicle[1][2], sceneVehicle[4][1]}))
				prueba->DrawText(-0.2f, -0.8f, "Press the LMB to mount", 0.01f);
			for (int i = 0; i < totalEnemies; i++) {				
				prueba->DrawText(-0.8f, 0.7f - (i * 0.1f), "ENEMY " + to_string(i) + ": " + to_string(*spiderEnemies[i]->getHealth()), 0.01f);
			}
			TurnOffAlphaBlending();


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

};
#endif