#pragma once

#include <stdio.h>
#include <conio.h>

// INCLUDE STATIC LIBRARIES
#ifdef _DEBUG
	#pragma comment (lib, "D3D11_Framework_d.lib")
#else
	#pragma comment (lib, "D3D11_Framework.lib")
#endif

// INCLUDE DIRECTX LIBRARIES
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")