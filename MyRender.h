// last revising at 20.01.22

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
		{
			Log::Get()->Error("MyRender::operator new(): can't allocate the memory");
			throw std::bad_alloc();
		}

		return ptr;
	}

	void operator delete(void* ptr)
	{
		MyRender* myRenderPtr = static_cast<MyRender*>(ptr);
		_aligned_free(myRenderPtr);
	}

private:
	ID3D11VertexShader*	m_pVertexShader;
	ID3D11Buffer*		m_pVertexBuffer;
	ID3D11InputLayout*	m_pVertexLayout;
	ID3D11PixelShader*	m_pPixelShader;
	ID3D11PixelShader*	m_pPixelShaderSolid;

	ID3D11Buffer*		m_pIndexBuffer;
	ID3D11Buffer*		m_pConstantBuffer;

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Projection;
};