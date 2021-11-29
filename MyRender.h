#pragma once

#include "D3D11_Framework.h"

using namespace D3D11Framework;

class MyRender : public Render
{
public:
	MyRender();
	bool Init(HWND hWnd);
	bool Draw(void);
	void Close(void);

private:
	HRESULT m_compileShaderFromFile(WCHAR* Filename, 
									LPCSTR EntryPoint,
									LPCSTR ShaderModel,
									ID3DBlob** ppBlobOut);
	
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
};
