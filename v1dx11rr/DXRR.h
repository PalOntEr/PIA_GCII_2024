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

class DXRR{	

private:
	int ancho;
	int alto;
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

	TerrenoRR *terreno;
	SkyDome *skydome;
	BillboardRR *billboard;
	Player *player;
	ModeloRR* model;
	ModeloRR* botella;
	ModeloRR* AntModel_Rigged_Smooth; 
	ModeloRR** Hojas;
	ModeloRR* pilarRoca1;
	ModeloRR* Tree1; 
	ModeloRR** Sticks; 
	ModeloRR** Crystals; 
	ModeloRR* BallRock;
	ModeloRR* House;
	ModeloRR* Cap;
	ModeloRR* Anthole;
	




	
	float izqder;
	float arriaba;
	float vel;
	float* velDir;
	bool breakpoint;
	vector2 uv1[32];
	vector2 uv2[32];
	vector2 uv3[32];
	vector2 uv4[32];

	XACTINDEX cueIndex;
	CXACT3Util m_XACT3;
	
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

		terreno = new TerrenoRR(300, 300, d3dDevice, d3dContext);
		skydome = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Assets/Skydomes/clear.jpg");
		//billboard = new BillboardRR(L"Assets/Billboards/fuego-anim.png",L"Assets/Billboards/fuego-anim-normal.png", d3dDevice, d3dContext, 5);
		//model = new ModeloRR(d3dDevice, d3dContext, "Assets/Cofre/Cofre.obj", L"Assets/Cofre/Cofre-color.png", L"Assets/Cofre/Cofre-spec.png", 0, 0);
		House = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Old_stone_house.obj", L"Assets/Textures/Old_stone_house_BaseColor.png", L"Assets/Textures/NoSpecular.png", 20, 0);
		Cap = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/cap.obj", L"Assets/Textures/cap.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		Tree1 = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Tree1.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		botella = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Botella.obj", L"Assets/Textures/Botella.png", L"Assets/Textures/NoSpecular.png", 10, 10);
		Anthole = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hormiguero.obj", L"Assets/Textures/Hormiguero.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		pilarRoca1 = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/pilarRoca1.obj", L"Assets/Textures/pilarRoca1_Color.png", L"Assets/Textures/NoSpecular.png", 65, 6);
		BallRock = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/ballRock.obj", L"Assets/Textures/ballRock.png", L"Assets/Textures/NoSpecular.png", 5, -5);
		AntModel_Rigged_Smooth = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/AntModel_Rigged_Smooth.obj", L"Assets/Textures/AntModel_Rigged_Smooth.png", L"Assets/Textures/NoSpecular.png", 0, 0);
		Crystals = new ModeloRR *[3];
		Crystals[0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Cristal1.obj", L"Assets/Textures/Cristal.png", L"Assets/Textures/NoSpecular.png", -14, -7);
		Crystals[1] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Cristal2.obj", L"Assets/Textures/Cristal.png", L"Assets/Textures/NoSpecular.png", 4, -17);
		Crystals[2] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Cristal3.obj", L"Assets/Textures/Cristal.png", L"Assets/Textures/NoSpecular.png", 14, 8);

		Hojas = new ModeloRR * [3]; 
		Hojas[0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hoja1.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", -8, 73);
		Hojas[1] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hoja2.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", -29, 54);
		Hojas[2] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Hoja3.obj", L"Assets/Textures/Hoja.png", L"Assets/Textures/NoSpecular.png", -36, 74);
		Sticks = new ModeloRR*[3]; 
		Sticks[0] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Stick1.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -14, 62);
		Sticks[1] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Stick2.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -18, 88);
		Sticks[2] = new ModeloRR(d3dDevice, d3dContext, "Assets/Models/Stick3.obj", L"Assets/Textures/Stick.png", L"Assets/Textures/NoSpecular.png", -39, 70);
		


		player = new Player(D3DXVECTOR3(0, 80, 0), Ancho, Alto);
		
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

		if (velDir)
			delete[] velDir;
		velDir = 0;

	}
	
	void Render(void)
	{
		float sphere[3] = { 0,0,0 };
		float prevPos[3] = { player->GetPosition().x, player->GetPosition().z, player->GetPosition().z};
		static float angle = 0.0f;
		angle += 0.005;
		if (angle >= 360) angle = 0.0f;
		bool collide = false;
		if( d3dContext == 0 )
			return;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView( backBufferTarget, clearColor );
		d3dContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		player->SetPosition(2, terreno->Superficie(player->GetPosition().x, player->GetPosition().z));
		player->MovePlayer(vel, velDir, arriaba, izqder);

		Camara* playerCamera = player->GetCamera(true);

		skydome->Update(playerCamera->vista, playerCamera->proyeccion);

		float camPosXZ[2] = { player->GetPosition().x, player->GetPosition().z };

		TurnOffDepth();
		skydome->Render(player->GetPosition());
		TurnOnDepth();	
		terreno->Draw(playerCamera->vista, playerCamera->proyeccion);
		//TurnOnAlphaBlending();
		//billboard->Draw(playerCamera->vista, playerCamera->proyeccion, player->GetPosition(),-11, -78, 4, 5, uv1, uv2, uv3, uv4, frameBillboard);

		//TurnOffAlphaBlending();

		float newPosition[3] = { 0.0f };

		//model->Draw(playerCamera->vista, playerCamera->proyeccion, terreno->Superficie(model->getPosX(), model->getPosZ()), player->GetPosition(), 10.0f, 0, 'A', 1);

		newPosition[0] = 55.0f;
		newPosition[1] = terreno->Superficie(botella->getPosX(), botella->getPosZ()) - 2.6f;
		newPosition[2] = -73.0f;
		botella->Draw(playerCamera->vista, playerCamera->proyeccion, newPosition, player->GetPosition(), 10.0f, 45, 'X', 1);

		newPosition[0] = player->GetPosition().x;
		newPosition[1] = player->GetPosition().y;
		newPosition[2] = player->GetPosition().z;
		D3DXVECTOR3 playerRef = player->GetFrontReference2D();
		float rotAngle = playerRef.z > 0 ? atanf(playerRef.x / playerRef.z) : atanf(playerRef.x / playerRef.z) + D3DX_PI;

		AntModel_Rigged_Smooth->Draw(playerCamera->vista, playerCamera->proyeccion, newPosition, player->GetPosition(), 10.0f, rotAngle, 'Y', 1);

		pilarRoca1->Draw(playerCamera->vista, playerCamera->proyeccion, terreno->Superficie(pilarRoca1->getPosX(), pilarRoca1->getPosZ()), player->GetPosition(), 10.0f, 0, 'A', 1);

		newPosition[0] = 95.0f;
		newPosition[1] = terreno->Superficie(House->getPosX(), House->getPosZ());
		newPosition[2] = -122.0f;
		House->Draw(playerCamera->vista, playerCamera->proyeccion, newPosition, player->GetPosition(), 10.0f, 0, 'A', 1);

		newPosition[0] = 0.0f;
		newPosition[1] = terreno->Superficie(Anthole->getPosX(), Anthole->getPosZ()) - 2.0f;
		newPosition[2] = 0.0f;
		Anthole->Draw(playerCamera->vista, playerCamera->proyeccion, newPosition, player->GetPosition(), 10.0f, 0, 'A', 4.0f);

		BallRock->Draw(playerCamera->vista, playerCamera->proyeccion, terreno->Superficie(BallRock->getPosX(), BallRock->getPosZ()), player->GetPosition(), 10.0f, 0, 'A', 1);

		newPosition[0] = -24.0f;
		newPosition[1] = terreno->Superficie(Cap->getPosX(), Cap->getPosZ());
		newPosition[2] = -28.0f;
		Cap->Draw(playerCamera->vista, playerCamera->proyeccion, newPosition, player->GetPosition(), 10.0f, 0, 'A', 0.1);

		newPosition[0] = -24.0f;
		newPosition[1] = terreno->Superficie(Tree1->getPosX(), Tree1->getPosZ());
		newPosition[2] = -28.0f;
		Tree1->Draw(playerCamera->vista, playerCamera->proyeccion, newPosition, player->GetPosition(), 10.0f, 0, 'A', 1.0);


		for (int i = 0; i < 3; i++)
			Hojas[i]->Draw(playerCamera->vista, playerCamera->proyeccion, terreno->Superficie(Hojas[i]->getPosX(), Hojas[i]->getPosZ()), player->GetPosition(), 10.0f, 0, 'A', 1);

		for (int i = 0; i < 3; i++)
			Crystals[i]->Draw(playerCamera->vista, playerCamera->proyeccion, terreno->Superficie(Crystals[i]->getPosX(), Crystals[i]->getPosZ()), player->GetPosition(), 10.0f, 0, 'A', 1);

		for (int i = 0; i < 3; i++) 
			Sticks[i]->Draw(playerCamera->vista, playerCamera->proyeccion, terreno->Superficie(Sticks[i]->getPosX(), Sticks[i]->getPosZ()), player->GetPosition(), 10.0f, 0, 'A', 1);

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

};
#endif