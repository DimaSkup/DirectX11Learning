#include "stdafx.h"

#include "macros.h"
#include "MyRender.h"
#include "MyInput.h"
#include <xnamath.h>

using namespace D3D11Framework;

/*
struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT4 vLightDir[3];
	XMFLOAT4 vLightColor[3];
	XMFLOAT4 vOutputColor;
};
*/

struct ConstantBuffer
{
	XMMATRIX WVP;
};

struct VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	//XMFLOAT3 Normal;
	//XMFLOAT4 Color;
};

MyRender::MyRender(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pPixelShaderSolid = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;

	m_pConstantBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	m_pTextureRV = nullptr;
	m_pSamplerLinear = nullptr;
	m_rot = 0.01;

	//m_camera = new Camera();
	Log::Get()->Debug("MyRender::MyRender()");
}

bool MyRender::Init(HWND hWnd)
{
	HRESULT hr = S_OK;
	

	// creation of the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	
	hr = m_compileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the vertex shader");
		return false;
	}

	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
										  pVSBlob->GetBufferSize(),
										  nullptr,
										  &m_pVertexShader);
	if (FAILED(hr))
	{
		_RELEASE(pVSBlob);
		Log::Get()->Error("MyRender::Init(): can't create the vertex shader OBJECT");
		return false;
	}

	
	// creation of the vertex input layout
	D3D11_INPUT_ELEMENT_DESC ied[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numInputElements = ARRAYSIZE(ied);

	hr = m_pd3dDevice->CreateInputLayout(ied, 
										 numInputElements,			 
										 pVSBlob->GetBufferPointer(),
										 pVSBlob->GetBufferSize(),
										 &m_pVertexLayout);
	_RELEASE(pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex input layout");
		return false;
	}

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	
	// creation of pixel shader for the main cube
	ID3DBlob* pPSBlob = nullptr;

	hr = m_compileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the pixel shader");
		return false;
	}

	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
										 pPSBlob->GetBufferSize(),
										 nullptr,
										 &m_pPixelShader);
	_RELEASE(pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the pixel shader OBJECT (main cube)");
		return false;
	}


	// definition of the cube params (coordinates and colours)
	/*
	VERTEX cube[] =
	{
		// the upper side
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(0, 0, 1, 1) },	// 0 
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT4(0, 1, 0, 1) },	// 1 
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT4(0, 1, 1, 1) },	// 2
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT4(1, 0, 0, 1) },	// 3

		// the bottom side
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1, 1, 0, 1) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT4(1, 0, 1, 1) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT4(1, 1, 1, 1) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT4(0, 0, 0, 1) },
	};
	*/

	/*
	// definition of the cube params (coordinates and normals)
	VERTEX cubes[] =
	{
		// the main cube
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f,  1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f,  1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f,  1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f,  1.0f, 0.0f) },
		
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },



		// the red light cube
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f,  -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },


		// the blue light cube
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	*/

	// definition of the cube params (vertices coordinates and texture coordinates)
	VERTEX cube[] =
	{
		// the upper and bottom sides
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },



		// the left and right sides
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },


		// the back and front sides
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },
	};

	UINT numVertices = ARRAYSIZE(cube);

	// creation of the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = sizeof(VERTEX) * numVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = cube;

	hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex buffer");
		return false;
	}

	UINT offset = 0;
	UINT stride = sizeof(VERTEX);
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// definition of the indices
	WORD indices[] =
	{
		// up
		3,1,0,
		2,1,3,

		// bottom
		4,6,7,
		5,6,4,

		// left
		9,11,10,
		8,11,9,

		// right
		12,14,15,
		13,14,12,


		// back
		20,22,23,
		21,22,20,

		// front
		17,19,18,
		16,19,17,
	};
	UINT numIndices = ARRAYSIZE(indices);
	Log::Get()->Print("NUM Vertices: %d", numVertices);
	Log::Get()->Print("NUM Indices: %d", numIndices);

	// creation of the index buffer
	bd.ByteWidth = sizeof(WORD) * numIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = indices;

	hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the index buffer");
		return false;
	}

	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


	// creation of the constant buffer
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the constant buffer");
		return false;
	}


	hr = D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, L"texture2.png",
												NULL, NULL,
												&m_pTextureRV, NULL);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create a shader resource view");
		return false;
	}

	
	// definition of the sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the sampler state");
		return false;
	}


	// definition of the matrices
	m_World1 = XMMatrixIdentity();
	m_World2 = XMMatrixIdentity();

	UINT width = Window::Get()->GetWidth();
	UINT height = Window::Get()->GetHeight();
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / height, 0.01f, 100.0f);
	
	return true;
}

/*
void MyRender::Update(void)
{
	static float t = 0;					// the current rotation angle
	static DWORD dwTimeStart = 0;		// time of the scene beginning
	DWORD dwTimeCur = GetTickCount();	// current time

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.f;	// calculation of the rotation angle

	m_World = XMMatrixRotationY(t);
}
*/

bool MyRender::Draw(void)
{
	m_rot += 0.0005f;
	if (m_rot == 6.26f)
		m_rot = 0.0f;

	m_View = Framework::GetCamera()->GetView();

	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX Rotation = XMMatrixRotationAxis(rotaxis, m_rot);
	XMMATRIX Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

	m_World1 = Translation * Rotation;

	Rotation = XMMatrixRotationAxis(rotaxis, -m_rot);
	XMMATRIX Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f);

	m_World2 = Rotation * Scale;

	XMMATRIX WVP = m_World1 * m_View * m_Projection;
	ConstantBuffer cb;
	cb.WVP = XMMatrixTranspose(WVP);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, nullptr, &cb, 0, 0);

	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureRV);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pImmediateContext->DrawIndexed(36, 0, 0);

	WVP = m_World2 * m_View * m_Projection;
	cb.WVP = XMMatrixTranspose(WVP);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, nullptr, &cb, 0, 0);

	m_pImmediateContext->DrawIndexed(36, 0, 0);

	return true;
}





















/*

bool MyRender::Draw()
{
	m_rot += 0.0005f;
	if (m_rot == 6.26f)
		m_rot = 0.0f;
	
	m_View = Framework::GetCamera()->GetView();
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX Rotation = XMMatrixRotationAxis(rotaxis, m_rot);
	XMMATRIX Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

	m_World1 = Translation * Rotation;

	Rotation = XMMatrixRotationAxis(rotaxis, -m_rot);
	XMMATRIX Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f);

	m_World2 = Rotation * Scale;

	XMMATRIX WVP = m_World1 * m_View * m_Projection;
	ConstantBuffer cb;
	cb.WVP = XMMatrixTranspose(WVP);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureRV);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pImmediateContext->DrawIndexed(36, 0, 0);

	WVP = m_World2 * m_View * m_Projection;
	cb.WVP = XMMatrixTranspose(WVP);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);
	m_pImmediateContext->DrawIndexed(36, 0, 0);

	return true;
}
*/


/*
bool MyRender::Draw(void)
{
	
	//Update();
	static float t = 0;					// the current rotation angle
	static DWORD dwTimeStart = 0;		// time of the scene beginning
	DWORD dwTimeCur = GetTickCount();	// current time

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.f;	// calculation of the rotation angle

	m_World = XMMatrixRotationY(t);
	

	XMFLOAT4 vLightDirs[3] = 
	{
		XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),	// blue light direction
		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),			// red light direction
		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),			// green light direction
	};

	XMFLOAT4 vLightColors[3] =
	{
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),			// blue colour
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),			// red colour
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),			// green colour
	};

	UINT numLightDirs = ARRAYSIZE(vLightDirs);

	// transformation of the red light direction
	XMMATRIX mRotate = XMMatrixRotationY(-2.0f * t);
	XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat4(&vLightDirs[1], vLightDir);

	// transformation of the green light direction
	mRotate = XMMatrixRotationX(-2.0f * t);
	vLightDir = XMLoadFloat4(&vLightDirs[2]);
	vLightDir = XMVector3Transform(vLightDir, mRotate);
	XMStoreFloat4(&vLightDirs[2], vLightDir);

	ConstantBuffer cb1;

	cb1.mWorld = XMMatrixTranspose(m_World);
	//cb1.mView = XMMatrixTranspose(m_camera->GetCameraView());
	
	cb1.mView = XMMatrixTranspose(GetCamera()->GetCameraView());
	cb1.mProjection = XMMatrixTranspose(m_Projection);
	cb1.vLightDir[0] = vLightDirs[0];
	cb1.vLightDir[1] = vLightDirs[1];
	cb1.vLightDir[2] = vLightDirs[2];
	cb1.vLightColor[0] = vLightColors[0];
	cb1.vLightColor[1] = vLightColors[1];
	cb1.vLightColor[2] = vLightColors[2];
	cb1.vOutputColor = XMFLOAT4(0, 1, 0, 1);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, nullptr, &cb1, 0, 0);

	// draw the cube
	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//m_pImmediateContext->PSSetShader(m_pPixelShaderSolid, nullptr, 0);

	m_pImmediateContext->DrawIndexed(36, 0, 0);


	// draw the light cubes
	for (UINT i = 0; i < numLightDirs; i++)
	{
		XMMATRIX mLightCube = XMMatrixScaling(0.3f, 0.3f, 0.3f);

		XMMATRIX mLightTrans = XMMatrixTranslationFromVector(5.0f * XMLoadFloat4(&vLightDirs[i]));

		m_World = mLightCube * mLightTrans;

		cb1.mWorld = XMMatrixTranspose(m_World);
		cb1.vOutputColor = vLightColors[i];
		m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, NULL, nullptr, &cb1, 0, 0);

		// draw the cube
		m_pImmediateContext->PSSetShader(m_pPixelShaderSolid, nullptr, 0);
		m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		m_pImmediateContext->DrawIndexed(36, 0, 0);

	}

	return true;
}
*/


void MyRender::Close(void)
{
	_RELEASE(m_pConstantBuffer);
	_RELEASE(m_pIndexBuffer);

	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pPixelShader);
	_RELEASE(m_pVertexBuffer);

	_RELEASE(m_pTextureRV);
	_RELEASE(m_pSamplerLinear);

	Log::Get()->Debug("MyRender::Close()");
}