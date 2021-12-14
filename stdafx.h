#pragma once


// INCLUDE STATIC LIBRARIES
#ifdef _DEBUG
	#pragma comment (lib, "MyD3D11Framework_d.lib")
#else
	#pragma comment (lib, "MyD3D11Framework.lib")
#endif

// INCLUDE DIRECTX LIBRARIES
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")