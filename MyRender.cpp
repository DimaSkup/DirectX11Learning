// last revising at 31.12.21

#include "MyRender.h"
#include "Log.h"

#include <d3dcompiler.h>
#include <type_traits>

using namespace D3D11Framework;

struct VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

MyRender::MyRender(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;

	m_pConstantBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

MyRender::~MyRender(void)
{

}

HRESULT MyRender::m_compileShaderFromFile(WCHAR* Filename, LPCSTR FunctionName,
											LPCSTR ShaderModel, ID3DBlob** OutBlob)
{
	HRESULT hr = S_OK;

	UINT shaderFlags = D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorMsg = nullptr;
	hr = D3DX11CompileFromFile(Filename, nullptr, NULL,
								FunctionName, ShaderModel, 
								shaderFlags, NULL, NULL,
								OutBlob, &pErrorMsg, NULL);

	if (FAILED(hr) && (pErrorMsg != nullptr))
		OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
	_RELEASE(pErrorMsg);

	return hr;
}





























/*

#include "stdafx.h"

#include "MyRender.h"
#include "Log.h"

#include <d3dcompiler.h>
#include <type_traits>

#include <vector>

using namespace D3D11Framework;

struct VERTEX
{
	VERTEX() :
		Pos(0.0f, 0.0f, 0.0f) {}
	VERTEX(float x, float y, float z) :
		Pos( x, y, z ) {}
	VERTEX(XMFLOAT3 vertex) :
		Pos(vertex) {}
	

	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};


class Circle
{
public:
	Circle(int cX, int cY, int rad, int det = 30) 
	{
		radius = rad;
		centerX = cX;
		centerY = cY;
		detalization = det;
		degree = 0;
		centerPoint = VERTEX(
			(centerX) / 100.0f - 0.5f,
			(centerY) / 100.0f - 0.5f,
			0.0f);

		calculateCircle();
	}

	auto GetVerticesData(void) const { return circleVertexBuffer.begin()._Ptr; }

private:
	void calculateCircle(void)
	{
		circleVertexBuffer.push_back(centerPoint);			// the first point: the centre of the circle
		circleVertexBuffer.push_back(calcPoint());			// the second point: upper
		circleVertexBuffer.push_back(calcPoint());			// the third point: right-bottom
		int indexOfPreviousVertex = circleVertexBuffer.size() - 1;
		
		for (UINT i = 1; i < 60; i++)
		{
			circleVertexBuffer.push_back(centerPoint);			// the center
			circleVertexBuffer.push_back(circleVertexBuffer[indexOfPreviousVertex]);	// the prev point on the circle	
			circleVertexBuffer.push_back(calcPoint());		// a new point on the circle
			indexOfPreviousVertex += 3;
		}
		
	}

	VERTEX calcPoint(void)
	{
		degree += 3.14159 / detalization;

		return VERTEX(
			(centerX + sin(degree) * radius) / 100.0f - 0.5f,
			(centerX + cos(degree) * radius) / 100.0f - 0.5f,
			0.0f);
	}

	VERTEX centerPoint;
	std::vector<VERTEX> circleVertexBuffer;
	int detalization;
	int radius;
	float centerX;
	float centerY;
	float degree;
};




// **********************************************************************

MyRender::MyRender(void)
{
	Log::Get()->Debug("MyRender::MyRender()");

	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;

	m_pIndexBuffer = nullptr;
	m_pConstantBuffer = nullptr;
}

MyRender::~MyRender(void)
{
	Log::Get()->Debug("MyRender::~MyRender()");
}

HRESULT MyRender::m_compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
											LPCSTR shaderModelType, ID3DBlob** ppShader)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorMessage;
	hr = D3DX11CompileFromFile(filename, NULL, NULL,
									functionName, shaderModelType,
									shaderFlags, NULL, NULL,
									ppShader, &pErrorMessage, NULL);
	if (FAILED(hr) && pErrorMessage != NULL)
		OutputDebugStringA((char*)pErrorMessage->GetBufferPointer());
	_RELEASE(pErrorMessage);


	Log::Get()->Debug("MyRender::m_compileShaderFromFile(): worked with %s shader file;"
						" function: %s()", filename, functionName);
	return hr;
}

bool MyRender::Init(HWND hWnd)
{
	HRESULT hr = S_OK;


	// compilation of the vertex shader and creation of the input layout per vertex data
	ID3DBlob* pVSBlob = NULL;
	hr = m_compileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the shader.fx file (vertex shader)");
		return false;
	}

	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
											pVSBlob->GetBufferSize(),
											NULL,
											&m_pVertexShader);
	if (FAILED(hr))
	{
		_RELEASE(pVSBlob);
		Log::Get()->Error("MyRender::Init(): can't create the vertex shader");
		return false;
	}



	// creation of the input vertex layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numLayoutElements = ARRAYSIZE(layout);

	hr = m_pd3dDevice->CreateInputLayout(layout, numLayoutElements, 
											pVSBlob->GetBufferPointer(),
											pVSBlob->GetBufferSize(),
											&m_pVertexLayout);
	_RELEASE(pVSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the input vertex layout");
		return false;
	}
	
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);


	// compilation of the pixel shader 
	ID3DBlob* pPSBlob = nullptr;
	hr = m_compileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the shader.fx file (pixel shader)");
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


	// set up the cube params
	VERTEX vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3( 1.0f, 1.0f, -1.0f),		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 1.0f, 1.0f,  1.0f),		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f,  1.0f),		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f),	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f),	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};


	// definition of the vertex buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = sizeof(VERTEX) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices;

	hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex buffer");
		return false;
	}


	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);


	// indices of the cube vertices
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};
	

	// INDEX BUFFER CREATION
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = indices;
	hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the indices buffer");
		return false;
	}


	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// CONSTANT BUFFER CREATION
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.CPUAccessFlags = 0;

	hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the constant buffer");
		return false;
	}


	// SPACES MATRICES DEFINITION
	m_World = XMMatrixIdentity();	// definition of the world matrix

	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);	// definition of the view matrix

	float width = 640.0f;
	float height = 480.0f;
	// definition of the projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / height, 0.01f, 100.0f);

	return true;
}

bool MyRender::Draw(void)
{	
	static float t = 0.0f;				// current rotation angle
	static DWORD dwTimeStart = 0;		// beginning time of the scene
	DWORD dwTimeCur = GetTickCount();	// time from the beginning of the scene

	if (dwTimeStart == 0)
	dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.f;

	// get world matrix by the current rotation angle
	m_World = XMMatrixRotationY(t);



	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->DrawIndexed(36, 0, 0);

	return true;
}

void MyRender::Close(void)
{
	m_pImmediateContext->ClearState();

	_RELEASE(m_pConstantBuffer);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pIndexBuffer);
	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexShader);
	_RELEASE(m_pPixelShader);

}

*/