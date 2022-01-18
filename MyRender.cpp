// last revising at 13.01.22

#include "stdafx.h"

#include "MyRender.h"
#include "Log.h"

#include <type_traits>

using namespace D3D11Framework;

struct VERTEX
{
	VERTEX(XMFLOAT3 vertex, XMFLOAT3 normal) :
		Pos(vertex), Normal(normal) {}

	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	//XMFLOAT4 Color;
};

/*

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

*/

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT4 vLightDir[2];
	XMFLOAT4 vLightColor[2];
	XMFLOAT4 vOutputColor;
};

MyRender::MyRender(void)
{
	Log::Get()->Debug("MyRender::MyRender()");

	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;

	m_pConstantBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pPixelShaderSolid = nullptr;
}

MyRender::~MyRender(void)
{

}


bool MyRender::Init(HWND hWnd)
{
	HRESULT hr = S_OK;

	// CREATION OF THE VERTEX SHADER
	ID3DBlob* pVertexShader = nullptr;
	 
	hr = m_compileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the vertex shader");
		_RELEASE(pVertexShader);
		return false;
	}

	hr = m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(),
											pVertexShader->GetBufferSize(),
											nullptr,
											&m_pVertexShader);
	if (FAILED(hr))
	{
		_RELEASE(pVertexShader);
		Log::Get()->Error("MyRender::Init(): can't create the vertex shader object");
		return false;
	}


	// CREATION OF THE INPUT VERTEX LAYOUT
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numInputElements = ARRAYSIZE(ied);

	hr = m_pd3dDevice->CreateInputLayout(ied, numInputElements, 
										 pVertexShader->GetBufferPointer(),
										 pVertexShader->GetBufferSize(),
										 &m_pVertexLayout);
	_RELEASE(pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the input vertex layout");
		return false;
	}

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);


	// CREATION OF THE PIXEL SHADER
	ID3DBlob* pPixelShader = nullptr;

	hr = m_compileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't compile the pixel shader");
		_RELEASE(pPixelShader);
		return false;
	}

	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(),
										 pPixelShader->GetBufferSize(),
										 nullptr,
										 &m_pPixelShader);
	_RELEASE(pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the pixel shader object");
		return false;
	}

	hr = m_compileShaderFromFile(L"shader.fx", "PSSolid", "ps_4_0", &pPixelShader);
	if (FAILED(hr))
		return false;

	hr = m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), 
										 pPixelShader->GetBufferSize(),
										 nullptr,
										 &m_pPixelShaderSolid);
	_RELEASE(pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the pixel shader SOLID object");
	}



	// DEFINITION OF THE MODEL DATA
	VERTEX cubes[] =
	{
		// THE FIRST CUBE (center)
		// the upper side
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),  XMFLOAT3( 0.0f,  1.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f, 1.0f, -1.0f),  XMFLOAT3( 0.0f,  1.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f, 1.0f,  1.0f),  XMFLOAT3( 0.0f,  1.0f,  0.0f ) },
		{ XMFLOAT3(-1.0f, 1.0f,  1.0f),  XMFLOAT3( 0.0f,  1.0f,  0.0f ) },

		// the bottom size
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3( 0.0f, -1.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT3( 0.0f, -1.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3( 0.0f, -1.0f,  0.0f ) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3( 0.0f, -1.0f,  0.0f ) },


		// THE SECOND CUBE (blue or red)
		// the left side
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(-1.0f,  0.0f,  0.0f ) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f,  0.0f,  0.0f ) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(-1.0f,  0.0f,  0.0f ) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(-1.0f,  0.0f,  0.0f ) },

		// the right side
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3( 1.0f,  0.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT3( 1.0f,  0.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3( 1.0f,  0.0f,  0.0f ) },
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3( 1.0f,  0.0f,  0.0f ) },

		// THE THIRD CUBE (blue or red)
		// the back side
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3( 0.0f,  0.0f, -1.0f ) },
		{ XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT3( 0.0f,  0.0f, -1.0f ) },
		{ XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3( 0.0f,  0.0f, -1.0f ) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3( 0.0f,  0.0f, -1.0f ) },

		// the front side
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3( 0.0f,  0.0f,  1.0f ) },
		{ XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3( 0.0f,  0.0f,  1.0f ) },
		{ XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3( 0.0f,  0.0f,  1.0f ) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3( 0.0f,  0.0f,  1.0f ) },
	};


	// CREATION OF THE VERTEX BUFFER
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = sizeof(VERTEX) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = cubes;

	hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the vertex buffer");
		return false;
	}

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// CREATION OF THE INDEX BUFFER
	WORD indices[] =
	{
	/*
		// the upper side
		3, 1, 0,
		2, 1, 3,

		// the front side
		2, 3, 7,
		6, 2, 7,

		// the right side
		6, 1, 2,
		5, 1, 6,

		// the left side
		4, 3, 0,
		7, 3, 4,

		// the back side
		5, 0, 1,
		4, 0, 5,

		// the bottom side
		4, 6, 7,
		5, 6, 4,
	*/

		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	// setting of the index buffer parameters
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = indices;

	hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the index buffer");
		return false;
	}

	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


	// CREATION OF THE CONSTANT BUFFER
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("MyRender::Init(): can't create the constant buffer");
		return false;
	}

	// SPACES MATRICES DEFINITION
	m_World = XMMatrixIdentity();	// definition of the world matrix	
	//m_World2 = XMMatrixIdentity();	

									// definition of the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -7.0f, 0.0f);
	XMVECTOR At  = XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f);
	XMVECTOR Up  = XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	// definition of the projection matrix
	float width = 640.0f;
	float height = 480.0f;
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / height, 0.01f, 100.0f);

	return true;
}


/*

void MyRender::Update(void)
{
	static float t = 0.0f;				// current rotation angle
	static DWORD dwTimeStart = 0;		// beginning time of the scene
	DWORD dwTimeCur = GetTickCount();	// time from the beginning of the scene

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;	// definition of the scene rotation angle


	m_World1 = XMMatrixRotationY(t);	// get world matrix by the current rotation angle

	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	XMMATRIX mRotation = XMMatrixRotationZ(-t);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mOrbit = XMMatrixRotationY(-t * 2.0f);


	m_World2 = mScale * mRotation * mTranslate * mOrbit;
}


*/


bool MyRender::Draw(void)
{
	static float t = 0.0f;				// current rotation angle
	static DWORD dwTimeStart = 0;		// beginning time of the scene
	DWORD dwTimeCur = GetTickCount();	// time from the beginning of the scene

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;	// definition of the scene rotation angle

	m_World = XMMatrixRotationY(t);	// get world matrix by the current rotation angle

	XMFLOAT4 vLightDirs[2] =
	{
		XMFLOAT4(-0.577f, 0.577, -0.577, 1.0f),	// blue light direction
		XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f),		// red light direction
	};
	
	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),		// blue colour
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),		// red colour
	};

	// operations to modify a direction of the red light
	XMMATRIX mRotate = XMMatrixRotationY(-2.0f * t);	// transformation matrix for a vector
	XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);	// load XMFLOAT4 into XMVECTOR
	vLightDir = XMVector3Transform(vLightDir, mRotate);	// transform the vector by the matrix
	XMStoreFloat4(&vLightDirs[1], vLightDir);			// load XMVECTOR into XMFLOAT4

	// setting up a constant buffer for the main cube drawing
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(m_World);
	cb1.mView = XMMatrixTranspose(m_View);
	cb1.mProjection = XMMatrixTranspose(m_Projection);

	cb1.vLightDir[0] = vLightDirs[0];	// blue light direction
	cb1.vLightDir[1] = vLightDirs[1];	// red light directon
	cb1.vLightColor[0] = vLightColors[0];	// blue colour
	cb1.vLightColor[1] = vLightColors[1];	// red colour
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);


	// draw the main cube
	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->DrawIndexed(36, 0, 0);


	// here we draw the cubes which show us the light direction and colour of the light source
	for (int m = 0; m < 2; m++)
	{
		// translate the cube position
		XMMATRIX mLightCube = XMMatrixTranslationFromVector(5.0f * XMLoadFloat4(&vLightDirs[m]));
		
		// get a smaller cube
		XMMATRIX mLightScaleCube = XMMatrixScaling(0.2f, 0.2f, 0.2f);
		mLightCube = mLightScaleCube * mLightCube;

		// setting of a constant buffer for the light cube
		cb1.mWorld = XMMatrixTranspose(mLightCube);
		cb1.vOutputColor = vLightColors[m];
		m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

		m_pImmediateContext->PSSetShader(m_pPixelShaderSolid, nullptr, 0);
		m_pImmediateContext->DrawIndexed(36, 0, 0);
	}

	return true;
}


void MyRender::Close(void)
{
	m_pImmediateContext->ClearState();

	_RELEASE(m_pConstantBuffer);
	_RELEASE(m_pIndexBuffer);
	_RELEASE(m_pVertexShader);
	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pPixelShader);
	_RELEASE(m_pPixelShaderSolid);
}