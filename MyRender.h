// last revising at 31.12.21

#pragma once

#include "D3D11_Framework.h"
#include <xnamath.h>

using namespace D3D11Framework;

class MyRender : public Render
{
public:
	MyRender(void);
	~MyRender(void);

	bool Init(HWND hWnd);
	bool Draw(void);
	void Close(void);

	void* operator new (size_t size)
	{
		void* ptr = _aligned_malloc(size, 16);

		if (!ptr)
			throw std::bad_alloc();

		return ptr;
	}

	void operator delete(void* ptr)
	{
		MyRender* myRenderPtr = static_cast<MyRender*>(ptr);
		_aligned_free(myRenderPtr);
	}

private:
	HRESULT m_compileShaderFromFile(WCHAR* Filename, LPCSTR FunctionName,
									LPCSTR ShaderModel, ID3DBlob** OutBlob);

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11Buffer*       m_pVertexBuffer;
	ID3D11InputLayout*  m_pVertexLayout;

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Projection;

	ID3D11Buffer*		m_pConstantBuffer;
	ID3D11Buffer*		m_pIndexBuffer;
};