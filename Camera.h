#pragma once

#include "Log.h"
#include <xnamath.h>

namespace D3D11Framework
{
	class Camera
	{
	public:
		Camera();

		XMMATRIX GetCameraView(void);

		void* operator new(size_t size)
		{
			void* ptr = _aligned_malloc(size, 16);

			if (!ptr)
			{
				Log::Get()->Error("Camera::operator new(): can't allocate the memory");
				throw std::bad_alloc();
			}

			return ptr;
		}

		void operator delete(void* ptr)
		{
			Camera* cPtr = static_cast<Camera*>(ptr);

			_aligned_free(cPtr);

			cPtr = nullptr;
			ptr = nullptr;
		}


	private:
		XMMATRIX m_View;
	};

	Camera::Camera(void)
	{
		XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -7.0f, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_View = XMMatrixLookAtLH(Eye, At, Up);

		Log::Get()->Debug("Camera::Camera()");
	}

	XMMATRIX Camera::GetCameraView(void)
	{
		return m_View;
	}
}


