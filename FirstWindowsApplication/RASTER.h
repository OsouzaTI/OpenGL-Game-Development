
// Compiler Directives
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

//Set the flag WIN32_LEAN_AND_MEAN to not
//exclude functions lower usage
#define WIN32_LEAN_AND_MEAN

#include <Windows.h> // Windows API 
#include <gl/GL.h> // GL functions
#include <gl/GLU.h> // GL tools


class RASTER
{
	public:
		//Device Context publico
		HDC hDC;
		//Handle to device context
		HGLRC glrc;
		//COnstrctor of RASTER
		RASTER();
		//Destructor of RASTER
		~RASTER();
		
		/// <summary>
		/// <para> Init to OpenGL</para>
		/// this function init the openGL with many parameters,
		///	please, read the comments in detail implementation.
		/// </summary>
		/// <param name="hWnd">window handle</param>
		/// <param name="color_bits">color bits</param>
		/// <param name="depth_bits">depth bits</param>
		/// <returns>boolean</returns>
		bool Init(HWND hWnd, unsigned char color_bits = 24,	unsigned char depth_bits = 32);
		//Release of OpenGL
		bool Release(HWND hWnd);
};

