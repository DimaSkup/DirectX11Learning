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
	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;

	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;

	ID3D11RenderTargetView* m_pRenderTargetView;
	
};
