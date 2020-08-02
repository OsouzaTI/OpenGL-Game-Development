#include "RASTER.h"

RASTER::RASTER() {
	//set device context to NULL
	hDC = NULL;
	//set handle to device context to null
	glrc = NULL;
}

RASTER::~RASTER() {
	//Destructor
}

bool RASTER::Init(HWND hWnd, unsigned char color_bits, unsigned char depth_bits){
	/*
		PIXELFORMATDESCRIPTOR:
			this is the structure to describe your
			need to get a Pixel Format

			this structure have 26 parameters,
			but most thers do not need.

	*/
	PIXELFORMATDESCRIPTOR pfd;
	// Pixel format variable to use for get a PixelFormat
	int PixelFormat;
	/*
		GetDC:
			use this method for the return a DeviceContext
			for the handle window.

	*/
	hDC = GetDC(hWnd);
	if (hDC == NULL)
	{
		MessageBox(hWnd, L"Error: Can't Get Device Context for Window",
			L"ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	/*		
	Metodo longo de preenchimento da estrutura
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = color_bits;
	pfd.cRedBits = 0;
	pfd.cRedShift = 0;
	pfd.cGreenBits = 0;
	pfd.cGreenShift = 0;
	pfd.cBlueBits = 0;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;
	pfd.cAlphaShift = 0;
	pfd.cAccumBits = 0;
	pfd.cAccumRedBits = 0;
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = depth_bits;
	pfd.cStencilBits = 0;
	pfd.cAuxBuffers = 0;
	pfd.iLayerType = 0;
	pfd.bReserved = 0;
	pfd.dwLayerMask = 0;
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;
	*/
	//memset: use this function to fill with a specific value
	memset(&pfd, 0, sizeof(pfd));
	// parameters your need in PIXELFORMATDESCRIPTOR
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = color_bits;
	pfd.cDepthBits = depth_bits;

	// use a pixelformat variable to return a FormatPixel using the
	// function ChoosePixelFormat
	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (PixelFormat == 0) {
		MessageBox(hWnd, L"Error: Can't Choose Pixel Format", L"ERROR", MB_OK | MB_ICONERROR);
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
		return false;
	}
	// try set the PixelFormat in device context
	if (SetPixelFormat(hDC, PixelFormat, &pfd) == 0) {
		MessageBox(hWnd, L"Error: Can't Set The Pixel Format", L"ERROR", MB_OK | MB_ICONERROR);
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
		return false;
	}
	/*
		if all success, try create a context openGL in
		device context
	*/
	glrc = wglCreateContext(hDC);
	if (glrc == NULL) {
		MessageBox(hWnd, L"Error: Can't Create GL Context", L"ERROR", MB_OK | MB_ICONERROR);
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
		return false;
	}
	// try to make a current gl context in thread application
	if (!wglMakeCurrent(hDC, glrc))
	{
		MessageBox(hWnd, L"Error: Can't Make Current GL Context", L"ERROR", MB_OK | MB_ICONERROR);
		wglDeleteContext(glrc);
		ReleaseDC(hWnd, hDC);
		glrc = NULL;
		hDC = NULL;
		return false;
	}
	// if the all function success, return true 
	return true;
}

// Liberation of contextGL
bool RASTER::Release(HWND hWnd)
{
	// if one of two parameters is NULL, return false
	if (hDC == NULL || glrc == NULL) return false;
	// reset the current context passing the two parameter NULL
	if (wglMakeCurrent(NULL, NULL)) {
		MessageBox(hWnd, L"Error: Release of DC And Rc Failed.", L"Release Error",
			MB_OK | MB_ICONERROR);
		return false;
	}
	// delete the context GL
	if (wglDeleteContext(glrc) == false)
	{
		MessageBox(hWnd, L"Error: Release Rendering Context Failed.", L"Release Error",
			MB_OK | MB_ICONERROR);
		return false;
	}
	glrc = NULL;
	// release device context of memory
	if (ReleaseDC(hWnd, hDC) == false)
	{
		MessageBox(hWnd, L"Error: Release Device Current Context Failed.", L"Release Error",
			MB_OK | MB_ICONERROR);
		return false;
	}
	hDC = NULL;
	// if the all function success, return true
	return true;
}