#ifndef _Text_
#define _Text_

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <xnamath.h>

using namespace std;

class Text {
private:
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11InputLayout* vertLayout;
	ID3D11Buffer* cbBuffer;

	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3dContext;

	ID3D11ShaderResourceView* Texture;
	ID3D11SamplerState* TexSamplerState;

	ID3D11Buffer* m_pColorCB;
	XMFLOAT4 m_Color;

	//Bitmap
	ID3D11Texture2D* texturaDescText;

	UINT numVertex;
	UINT numIndex;

	float Height;
	float Width;

	//The constant buffer Structure
	struct ConstantBuffer
	{
		D3DXMATRIX WorldMatrix;
	} consBuff;

	//The vertex Structure
	struct SimpleVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 tex;
	};

	SimpleVertex* vertices;

public:
	Text(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3dContext, float Height, float Width, LPCWSTR texturePath, XMFLOAT4 color) {
		this->Height = Height;
		this->Width = Width;
		this->m_Color = color;

		InitScene(d3d11Device, d3dContext, texturePath);
		createBitMap();
	}

	~Text()
	{
		//libera recursos
		UnloadContent();
	}

	bool InitScene(ID3D11Device* D3D11Device, ID3D11DeviceContext* D3DContext, LPCWSTR texturePath)
	{
		// Inicializamos variables
		d3d11Device = D3D11Device;
		d3dContext = D3DContext;

		HRESULT d3dResult;

		//The input-layout description
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);
		LPCWSTR shaderFile = L"Text.fx";

		//Compile Shaders from shader file
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		ID3DBlob* errorBuffer = 0;
		d3dResult = D3DX11CompileFromFile(shaderFile, 0, 0, "VS_Main", "vs_4_0", shaderFlags, 0, 0, &VS_Buffer, &errorBuffer, 0);
		if (FAILED(d3dResult))
		{
			return false;
		}
		d3dResult = D3DX11CompileFromFile(shaderFile, 0, 0, "PS_Main", "ps_4_0", shaderFlags, 0, 0, &PS_Buffer, &errorBuffer, 0);
		if (FAILED(d3dResult))
		{
			return false;
		}

		//Create the Shader Objects
		d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), NULL, &VS);
		d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(),
			PS_Buffer->GetBufferSize(), NULL, &PS);

		//Create the Input Layout
		d3d11Device->CreateInputLayout(layout, numElements,
			VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &vertLayout);

		//Free Buffers Space
		VS_Buffer->Release();
		VS_Buffer = NULL;
		PS_Buffer->Release();
		PS_Buffer = NULL;

		//Set the Input Layout
		d3dContext->IASetInputLayout(vertLayout);

		//Set Primitive Topology
		d3dContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



		//Create the buffer to send to the cbuffer in effect file
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof(ConstantBuffer);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;

		d3d11Device->CreateBuffer(&cbbd, NULL,
			&cbBuffer);

		// Set the texture
		D3DX11CreateShaderResourceViewFromFile(d3d11Device, texturePath,
			NULL, NULL, &Texture, NULL);

		// Describe the Sample State
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//Create the Sample State
		d3dResult = d3d11Device->CreateSamplerState(&sampDesc, &TexSamplerState);
		if (FAILED(d3dResult))
		{
			return false;
		}

		//Color Buffer
		D3D11_BUFFER_DESC constBufferDesc;
		::ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.ByteWidth = sizeof(XMFLOAT4);
		constBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = d3d11Device->CreateBuffer(&constBufferDesc, 0, &m_pColorCB);
		if (FAILED(d3dResult)) {
			return false;
		}
	}

	void createBitMap() {

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		// Textura del bitmap para Text 

		// Inicialisamos la descripcion de la textura.
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Llenamos la descripcion de la textura
		textureDesc.Width = Width / 2;
		textureDesc.Height = Height / 2;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// creamos la textura con el device
		d3d11Device->CreateTexture2D(&textureDesc, NULL, &texturaDescText);


		Height = Height / 100.0f;
		Width = Width / 100.0f;
		//Create the vertex buffer
		setVertex(0.0f, 0.03125f, 0.34f, 0.68f);

		DWORD indices[] = {
			0, 1, 2,
			0, 2, 3
		};
		numIndex = ARRAYSIZE(indices);

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc,
			sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndex;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = indices;
		d3d11Device->CreateBuffer(&indexBufferDesc,
			&iinitData,
			&IndexBuffer);
	}

	void setVertex(float uLeft, float uRight, float vTop, float vBottom) {
		SimpleVertex vertices[] =
		{
			{ D3DXVECTOR3(-Width, -Height, 0), D3DXVECTOR2(uLeft, vBottom) },
			{ D3DXVECTOR3(-Width, Height, 0), D3DXVECTOR2(uLeft, vTop) },
			{ D3DXVECTOR3(Width, Height, 0), D3DXVECTOR2(uRight, vTop) },
			{ D3DXVECTOR3(Width, -Height, 0), D3DXVECTOR2(uRight, vBottom) },
		};

		numVertex = ARRAYSIZE(vertices);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * numVertex;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = vertices;
		d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &VertexBuffer);
	}

	bool UnloadContent()
	{
		if (VertexBuffer)
			VertexBuffer->Release();
		VertexBuffer = NULL;
		if (IndexBuffer)
			IndexBuffer->Release();
		IndexBuffer = NULL;
		if (VS)
			VS->Release();
		VS = NULL;
		if (PS)
			PS->Release();
		PS = NULL;
		if (VS_Buffer)
			VS_Buffer->Release();
		VS_Buffer = NULL;
		if (PS_Buffer)
			PS_Buffer->Release();
		PS_Buffer = NULL;
		if (vertLayout)
			vertLayout->Release();
		vertLayout = NULL;
		if (cbBuffer)
			cbBuffer->Release();
		cbBuffer = NULL;
		if (Texture)
			Texture->Release();
		Texture = NULL;
		if (TexSamplerState)
			TexSamplerState->Release();
		TexSamplerState = NULL;
		if (m_pColorCB)
			m_pColorCB->Release();
		m_pColorCB = NULL;
	}

	void Draw(float posX, float posY) {

		// Dibujar la Text

		/*Aqui se dibuja el bitmap, solo necesitamos un cuadrado donde poner la textura, por eso utilizamos los
		vertices de el billboard, lo estaremos dibujando directamente sobre la pantalla, recuerden que las
		coordenadas 0,0 son el centro de la pantalla, los lados van de -1 a 1*/

		d3dContext->VSSetShader(VS, 0, 0);
		d3dContext->PSSetShader(PS, 0, 0);

		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		//Cargamos el buffer de indices y vertices
		d3dContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		d3dContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Seteamos la matriz WVP a una escala y traslacion dada apara acomodarla en pantalla
		D3DXMATRIX Scale, Translation, World;
		D3DXMatrixScaling(&Scale, 1, 1, 1);
		D3DXMatrixTranslation(&Translation, posX, posY, 0.8);
		World = Scale * Translation;

		D3DXMatrixTranspose(&consBuff.WorldMatrix, &World);

		d3dContext->UpdateSubresource(cbBuffer, 0, NULL, &consBuff, 0, 0);
		d3dContext->VSSetConstantBuffers(0, 1, &cbBuffer);

		d3dContext->UpdateSubresource(m_pColorCB, 0, 0, &m_Color, 0, 0);

		d3dContext->PSSetShaderResources(0, 1, &Texture);
		d3dContext->PSSetSamplers(0, 1, &TexSamplerState);
		d3dContext->PSSetConstantBuffers(1, 1, &m_pColorCB);

		//Dibujamos el bitmap
		d3dContext->DrawIndexed(numIndex, 0, 0);
	}

	void DrawText(float posX, float posY, string texto, float adjustment) {
		// Declarar variables e intervalos
		float uStep = 0.03125;
		float vStep = 0.33334;

		float uLeft, uRight, vTop, vBottom;
		// Calcular la posición de la letra en la textura
		for (int i = 0; i < texto.length(); i++) {
			int ascii = int(texto.at(i));

			if (ascii <= 63) {
				vTop = 0;
				vBottom = vStep;

				ascii -= 32;
			}
			else if (ascii <= 95) {
				vTop = vStep;
				vBottom = vStep * 2;

				ascii -= 64;
			}
			else {
				vTop = vStep * 2;
				vBottom = 1;
				//vBottom = vStep * 3;

				ascii -= 96;
			}

			uLeft = ascii * uStep;
			uRight = (ascii * uStep) + uStep;

			// Actulizar valor de cada letra
			setVertex(uLeft, uRight, vTop, vBottom);
			Draw(posX + (i * (Width + adjustment)), posY);
		}	
	}

	string Time(int seconds) {
		int minutes = seconds / 60;
		int remainingSecons = seconds % 60;

		stringstream minutesSs, secondsSs;
		minutesSs << minutes;
		secondsSs << remainingSecons;

		string minutesStr = minutesSs.str();
		string secondsStr = secondsSs.str();

		if (remainingSecons < 10) {
			secondsStr = "0" + secondsStr;
		}

		return minutesStr + ":" + secondsStr;
	}
};

#endif _Text_