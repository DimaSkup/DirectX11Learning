#include "stdafx.h"

#include "MyRender.h"
#include "Log.h"

#include <xnamath.h>
#include <d3dcompiler.h>

using namespace D3D11Framework;

struct VERTEX
{
	XMFLOAT3 Pos;
};

MyRender::MyRender(void)
{
	Log::Get()->Debug("MyRender::MyRender()");

	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;
}

MyRender::~MyRender(void)
{
	Log::Get()->Debug("MyRender::~MyRender()");
}

HRESULT MyRender::m_compileShaderFromFile(wchar_t* filename, char* functionName,
											char* shaderModelType, ID3DBlob** ppShader, 
											ID3DBlob** ppErrMsg)
{
	Log::Get()->Debug("MyRender::m_compileShaderFromFile(): the begin");
	HRESULT hr = S_OK;

	hr = D3DX11CompileFromFile(filename, NULL, NULL,
									functionName, shaderModelType,
									D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS,
									NULL, NULL,
									ppShader, ppErrMsg, NULL);
	
	Log::Get()->Debug("MyRender::m_compileShaderFromFile(): the end");
	return hr;
}

bool MyRender::Init(HWND hWnd)
{
	Log::Get()->Debug("MyRender::Init()");
	HRESULT hr = S_OK;
	ID3DBlob* pVSBlob = nullptr;		// byte code of the vertex shader
	ID3DBlob* pPSBlob = nullptr;		// byte code of the pixel shader
	ID3DBlob* pErrorMsg = nullptr;	// here we put errors which occur 
									// during shader compilation from file

	wchar_t* shaderFileName = L"shader.fx";

	// -----------------------------------
	//		VERTEX SHADER
	// -----------------------------------

	// compilation of the vertex shader
	hr = m_compileShaderFromFile(shaderFileName, "VS", "vs_4_0", &pVSBlob, &pErrorMsg);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the vertex shader");
		if (pErrorMsg)
		{
			OutputDebugStringA((LPCSTR)pErrorMsg->GetBufferPointer());
			_RELEASE(pErrorMsg);
		}
		
		return false;
	}
	
	// incapsulate the vertex shader into the vertex shader object
	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), 
											pVSBlob->GetBufferSize(),
											NULL, &m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex shader");
		_RELEASE(pVSBlob);
		return false;
	}


	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, NULL);
	Log::Get()->Debug("MyRender::Init(): the vertex shader is compiled successfully");

	// -----------------------------------
	//		PIXEL SHADER
	// -----------------------------------

	hr = m_compileShaderFromFile(shaderFileName, "PS", "ps_4_0", &pPSBlob, &pErrorMsg);
	if (FAILED(hr))
	{
		if (pErrorMsg)
		{
			OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
			_RELEASE(pErrorMsg);
		}

		_RELEASE(pPSBlob);
		Log::Get()->Error("MyRender::Init(): can't compile the pixel shader");
		return false;
	}

	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
											pPSBlob->GetBufferSize(),
											NULL,
											&m_pPixelShader);
	_RELEASE(pPSBlob);
	if (FAILED(hr))
	{
		
		Log::Get()->Error("MyRender::Init(): can't create the pixel shader");
		return false;
	}

	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, NULL);
	Log::Get()->Debug("MyRender::Init(): the pixel shader is compiled successfully");


	// -----------------------------------
	//		INPUT VERTEX LAYOUT
	// -----------------------------------
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(ied);

	hr = m_pd3dDevice->CreateInputLayout(ied, 1, 
									pVSBlob->GetBufferPointer(), 
									pVSBlob->GetBufferSize(), 
									&m_pVertexLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex layout");
		return false;
	}

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
	Log::Get()->Debug("MyRender::Init(): the input layout is created successfully");


	// -----------------------------------
	//		VERTEX BUFFER
	// -----------------------------------

	// triangle data
	VERTEX triangle[] = 
	{
		{ XMFLOAT3(0.0f,  0.5f,  0.0f) },
		{ XMFLOAT3(0.5f,  -0.5f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = sizeof(VERTEX) * 3;		
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex buffer");
		return false;
	}

	Log::Get()->Debug("MyRender::Init(): the vertex buffer is created successfully");

	D3D11_MAPPED_SUBRESOURCE ms;
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, triangle, sizeof(triangle));
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);
	Log::Get()->Debug("MyRender::Init(): the vertex buffer is filled with data successfully");

	return true;
}

bool MyRender::Draw(void)
{
	UINT offset = 0;
	UINT stride = sizeof(VERTEX);
	
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pImmediateContext->Draw(3, 0);
	
	return true;
}

void MyRender::Close(void)
{
	m_pImmediateContext->ClearState();

	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pVertexShader);
	_RELEASE(m_pPixelShader);
}