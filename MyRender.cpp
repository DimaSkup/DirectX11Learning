// last revising at 01.12.21

#include "stdafx.h"
#include "MyRender.h"
#include <xnamath.h>
#include <d3dcompiler.h>

struct SimpleVertex
{
	XMFLOAT3 Pos;
};

MyRender::MyRender(void)
{
	Log::Get()->Debug("MyRender::MyRender()");

	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
}


HRESULT MyRender::m_compileShaderFromFile(WCHAR* Filename, LPCSTR EntryPoint,
								LPCSTR ShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	hr = D3DX11CompileFromFile(Filename, NULL, NULL,
								EntryPoint, ShaderModel, NULL, NULL,
								NULL, ppBlobOut, NULL, NULL);

	return hr;
}

bool MyRender::Init(HWND hWnd)
{
	HRESULT hr = S_OK;

	// ---------------------- THE VERTEX SHADER INITIALIZATION ---------------------
	ID3DBlob* pVSBlob = nullptr;

	// compilation of the vertex shader into a byte code
	hr = m_compileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't compile the vertex shader. "
			"Please, run the program from the directory which contains the file");

		return false;
	}

	// incapsulation the vertex shader into a vertex shader object
	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), 
											pVSBlob->GetBufferSize(),
											NULL,
											&m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create a vertex shader object");
		_RELEASE(pVSBlob);
		return false;
	}



	// ---------------------- THE INPUT LAYOUT --------------------------

	// definition of the input layout description
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numLayoutDesc = ARRAYSIZE(layoutDesc);

	// creation of the input layout
	hr = m_pd3dDevice->CreateInputLayout(layoutDesc, numLayoutDesc,
									pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
									&m_pVertexLayout);

	_RELEASE(pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the input layout");
		return false;
	}

	// setting of the input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);




	// ------------------------ THE PIXEL SHADER --------------------------------
	ID3DBlob* pPSBlob = nullptr;

	// compilation of the pixel shader into a byte code
	hr = m_compileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't compile the pixel shader. "
			"Please, run the program from the directory which contains the file");
		return false;
	}

	// incapsulation of the pixel shader into the pixel shader object
	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
									pPSBlob->GetBufferSize(),
									NULL,
									&m_pPixelShader);
	_RELEASE(pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the pixel shader object");
		return false;
	}


	// --------------------- THE MODEL DATA -------------------
	SimpleVertex triangle[] =
	{
		XMFLOAT3(0.0f, 0.5f, 0.0f),
		XMFLOAT3(0.5f, -0.5f, 0.0f),
		XMFLOAT3(-0.5f, -0.5f, 0.0f),
	};

	// definition of the vertex buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// fill in the vertex buffer with the triangle's data
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = triangle;

	// creation of the vertex buffer
	hr = m_pd3dDevice->CreateBuffer(&bd, &data, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Err("MyRender::Init(): can't create the vertex buffer");
		return false;
	}


	// setting of the vertex buffer
	UINT offset = 0;
	UINT stride = sizeof(SimpleVertex);
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// setting of the primitive topology
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool MyRender::Draw(void)
{
	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->Draw(3, 0);

	return true;
}

void MyRender::Close(void)
{
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexShader);
	_RELEASE(m_pPixelShader);
}