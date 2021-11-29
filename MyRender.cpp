#include "stdafx.h"
#include "MyRender.h"
#include <xnamath.h>
#include <d3dcompiler.h>

struct SimpleVertex
{
	XMFLOAT3 Pox;
};

MyRender::MyRender()
{
	Log::Get()->Debug("MyRender::MyRender()");
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexLayout = nullptr;
	m_pVertexBuffer = nullptr;
}

HRESULT MyRender::m_compileShaderFromFile(WCHAR* Filename,
											LPCSTR EntryPoint,
											LPCSTR ShaderModel,
											ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	hr = D3DX11CompileFromFile(Filename, NULL, NULL,
								EntryPoint, ShaderModel, 0, 0,
								NULL, ppBlobOut, NULL, NULL);

	return hr;
}

bool MyRender::Init(HWND hWnd)
{
	HRESULT hr = S_OK;


	// ----------------------------- THE VERTEX SHADER ----------------------

	// Compiling of the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = m_compileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't compile file shader.fx. "
			"Please, run this program from the folder, which contains this file");
		return false;
	}

	// Creation of the vertex shader
	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), 
											pVSBlob->GetBufferSize(),
											NULL,
											&m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the vertex shader");
		_RELEASE(pVSBlob);
		return false;
	}



	// Initialization of the input layout description
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = ARRAYSIZE(layout);

	// creationf of the input layout
	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, 
											pVSBlob->GetBufferPointer(), 
											pVSBlob->GetBufferSize(), 
											&m_pVertexLayout);

	_RELEASE(pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the input layout");
		return false;
	}

	// setting of the input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);





	// --------------------- THE PIXEL SHADER ---------------------------
	ID3DBlob* pPSBlob = NULL;
	hr = m_compileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init():  can't compile file shader.fx. "
			"Please, run this program from the folder, which contains this file");
		return false;
	}

	// Creation of the pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), 
											pPSBlob->GetBufferSize(), 
											NULL,
											&m_pPixelShader);
	_RELEASE(pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the pixel shader");
		return false;
	}




	// --------------------- THE MODEL DATA ---------------------------
	// Creation of the vertex buffer
	SimpleVertex vertices[] =
	{
		XMFLOAT3(0.0f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f),
	};


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertices;

	hr = m_pd3dDevice->CreateBuffer(&bd, &data, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the vertex buffer");
		return false;
	}

	// Setting of the vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// setting of the primitive topology
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool MyRender::Draw()
{
	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->Draw(3, 0);

	return true;
}

void MyRender::Close()
{
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexShader);
	_RELEASE(m_pPixelShader);
}
