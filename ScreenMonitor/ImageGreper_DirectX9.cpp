#include "stdafx.h"
#include "ImageGreper_DirectX9.h"

#include "ImageGreper.h"

#include <Windows.h>
#include <Wincodec.h>             // we use WIC for saving images
#include <d3d9.h>                 // DirectX 9 header
#pragma comment(lib, "d3d9.lib")  // link to DirectX 9 library

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);goto cleanup;}}
#define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

CImageGreper_DirectX9::CImageGreper_DirectX9()
{
}


CImageGreper_DirectX9::~CImageGreper_DirectX9()
{
}

// override
void CImageGreper_DirectX9::ThreadMain()
{
	while (m_atombThreadRun)
	{
		HRESULT hr = Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT, 1);
		Sleep(m_Config.nDuration);
	}
}

// override
cv::Mat CImageGreper_DirectX9::TakeAScreenShot()
{
	Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT, 1, false);
	return m_matImageForOneShot;
}

cv::Mat CImageGreper_DirectX9::ConvertImage(const UINT& unWidth,
	const UINT& unHeight,
	const UINT& unStride,
	const LPBYTE& pbPixels)
{
	//cv::imwrite("ConvertImage.jpg", cv::Mat(unHeight, unWidth, CV_8UC4, (unsigned*)pbPixels));

	// opencv will create a Mat without copying from the original array, so when the array is deleted,
	// this Mat is empty, and user cannot release the memory by releasing the Mat in this case
	return cv::Mat(unHeight, unWidth, CV_8UC4, (unsigned*)pbPixels);
}

HRESULT CImageGreper_DirectX9::SavePixelsToFile32bppPBGRA(UINT width,
	UINT height,
	UINT stride,
	LPBYTE pixels,
	LPWSTR filePath,
	const GUID &format)
{
	if (!filePath || !pixels)
		return E_INVALIDARG;

	HRESULT hr = S_OK;
	IWICImagingFactory *factory = nullptr;
	IWICBitmapEncoder *encoder = nullptr;
	IWICBitmapFrameEncode *frame = nullptr;
	IWICStream *stream = nullptr;
	GUID pf = GUID_WICPixelFormat32bppPBGRA;
	BOOL coInit = CoInitialize(nullptr);

	HRCHECK(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));
	HRCHECK(factory->CreateStream(&stream));
	HRCHECK(stream->InitializeFromFilename(filePath, GENERIC_WRITE));
	HRCHECK(factory->CreateEncoder(format, nullptr, &encoder));
	HRCHECK(encoder->Initialize(stream, WICBitmapEncoderNoCache));
	HRCHECK(encoder->CreateNewFrame(&frame, nullptr)); // we don't use options here
	HRCHECK(frame->Initialize(nullptr)); // we dont' use any options here
	HRCHECK(frame->SetSize(width, height));
	HRCHECK(frame->SetPixelFormat(&pf));
	HRCHECK(frame->WritePixels(height, stride, stride * height, pixels));
	HRCHECK(frame->Commit());
	HRCHECK(encoder->Commit());

cleanup:
	RELEASE(stream);
	RELEASE(frame);
	RELEASE(encoder);
	RELEASE(factory);
	if (coInit) CoUninitialize();
	return hr;
}

HRESULT CImageGreper_DirectX9::Direct3D9TakeScreenshots(UINT adapter, UINT count, bool bUpdateObservers)
{
	HRESULT hr = S_OK;
	IDirect3D9 *d3d = nullptr;
	IDirect3DDevice9 *device = nullptr;
	IDirect3DSurface9 *surface = nullptr;
	D3DPRESENT_PARAMETERS parameters = { 0 };
	D3DDISPLAYMODE mode;
	D3DLOCKED_RECT rc;
	UINT pitch;
	SYSTEMTIME st;
	LPBYTE *shots = nullptr;

	// init D3D and get screen size
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	HRCHECK(d3d->GetAdapterDisplayMode(adapter, &mode));

	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = mode.Height;
	parameters.BackBufferWidth = mode.Width;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;

	// create device & capture surface
	HRCHECK(d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device));
	HRCHECK(device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr));

	// compute the required buffer size
	HRCHECK(surface->LockRect(&rc, NULL, 0));
	pitch = rc.Pitch;
	HRCHECK(surface->UnlockRect());

	// allocate screenshots buffers
	shots = new LPBYTE[count];
	for (UINT i = 0; i < count; i++)
	{
		shots[i] = new BYTE[pitch * mode.Height];
	}

	GetSystemTime(&st); // measure the time we spend doing <count> captures	
	char temp[1024];
	memset(temp, 0, 1024);
	sprintf_s(temp, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);
	if (m_strPreviousTime != temp)
	{
		wprintf(L"FPS: %i\n", m_nCaptureCounter);
		m_nCaptureCounter = 0;
	}
	m_strPreviousTime = temp;
	memset(temp, 0, 1024);
	m_nCaptureCounter++;

	for (UINT i = 0; i < count; i++)
	{
		// get the data
		HRCHECK(device->GetFrontBufferData(0, surface));

		// copy it into our buffers
		HRCHECK(surface->LockRect(&rc, NULL, 0));
		CopyMemory(shots[i], rc.pBits, rc.Pitch * mode.Height);
		HRCHECK(surface->UnlockRect());
	}
	//GetSystemTime(&st);
	//wprintf(L"%i:%i:%i.%i\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// save all screenshots
	for (UINT i = 0; i < count; i++)
	{
		WCHAR file[100];
		wsprintf(file, L"cap%i.png", i);
		//HRCHECK(SavePixelsToFile32bppPBGRA(mode.Width, mode.Height, pitch, shots[i], file, GUID_ContainerFormatPng));
		sprintf_s(temp, "%04d%02d%02d_%02d%02d%02d_%04d", st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		std::string strTimeStamp(temp);
		memset(temp, 0, 1024);
		if (bUpdateObservers)
		{
			UpdateObserver(ConvertImage(mode.Width, mode.Height, pitch, shots[i]), strTimeStamp);
		}
		else
		{
			// We have to copy the Mat to internal memory because this Mat is created from an array.
			// Mat in this case did not has its own memory.
			// The content will be deleted when the original array is deleted.
			// So we have to copy the content to m_matImageForOneShot if we want to use it outside from this function
			ConvertImage(mode.Width, mode.Height, pitch, shots[i]).copyTo(m_matImageForOneShot);
		}
	}

cleanup:
	if (shots != nullptr)
	{
		for (UINT i = 0; i < count; i++)
		{
			delete shots[i];
		}
		delete[] shots;
	}
	RELEASE(surface);
	RELEASE(device);
	RELEASE(d3d);
	return hr;
}