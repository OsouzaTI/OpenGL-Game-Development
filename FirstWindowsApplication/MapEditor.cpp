#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

#include "RASTER.h"
#include "MapFormat.h"

#include "resource.h"

#define DEFAULT_BUTTON_WIDTH	100
#define DEFAULT_BUTTON_HEIGHT	20

HINSTANCE	GlobalInstance;
HMENU		Menu;
HMENU		PopupMenu;
HWND		Window;
HWND		bCreateWall;
HWND		RenderWindow;
RASTER		raster;

//-- START TODOS --//
/*
	[]	comment the functions	(1/9)
	[*] comment the classes		(1/1)
	[]	comment the structs		(0/?)
	[] ...
*/
//-- END TODOS --//


// Prototypes functions
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MapDetailsDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WMCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WMSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ResizeGLWindow(long width, long height);
void DisplayPopupMenu(long x, long y);
void SetGLDefaults();
void Render();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPSTR lpCmdString, int CmdShow) {
	// Definition the GlobalInstance with hInstance of Window Main	
	GlobalInstance = hInstance;
	// Variable to get a dimensions of window main
	RECT rect;
	// Struct to atributes for the register class
	WNDCLASS wc; // WNDCLASS = Window Class
	// the buffer usage to get messages of window main
	MSG msg;

	/* filling the struct WNDCLASS for the register window */
	// defined the cbClsExtra with 0, because we do not allocate
	// extra bytes of memory beyond the WNDCLASS size.
	wc.cbClsExtra = 0; 
	// defined the cbWndExtra to 0, because we do not allocate
	// extra memory for the window instance.
	wc.cbWndExtra = 0;
	/*
		the variable hbrBackground sets the background color
		for the window, we use the function GetStockObject to
		retrieve a predefined stock brush, and cast it as an
		HBRUSH type. the brush type is a handle to a brush object.
	*/
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	// the hCursor variable set the cursor for the window
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// the hIcon sets the default icon for the application
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//the hInstance variable requires the handle of this instance
	wc.hInstance = hInstance;
	// the lpfnWndProc variable is the long pointer to the
	// function window procedure
	wc.lpfnWndProc = WndProc;
	// the lpszClassName variablre is the name of the class window
	wc.lpszClassName = L"ME";
	// the lpszMenuName variable is menu to be loaded for the window class
	wc.lpszMenuName = NULL;
	//this variable is the style for window class
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	// try register window class with function RegisterClass
	if (!RegisterClass(&wc)) {
		return false;
	}

	// if the window success in register , create a window
	// with the function CreateWindow
	Window = CreateWindow(
		L"ME",
		L"Map Editor",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 640, 480, NULL, NULL, hInstance, NULL);

	// if window create failed return program false
	if (!Window) return 0;

	//this function return a bool, is success the bool is true and
	//the second parameter of type RECT recieve the object
	GetClientRect(Window, &rect);

	//if all functions obtain success, create a window static to 
	//render window class, this window is a child of window main class
	RenderWindow = CreateWindow(
		L"STATIC",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		DEFAULT_BUTTON_WIDTH, 0,
		rect.right - rect.left - DEFAULT_BUTTON_WIDTH,
		rect.bottom - rect.top, Window, NULL, hInstance, NULL);

	// simple buttom
	bCreateWall = CreateWindow(
		L"BUTTON",
		L"Create Wall",
		WS_CHILD | WS_VISIBLE,
		0, 100,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);

	//the Menu varible datatype HWND is the menu window
	Menu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(Window, Menu); // set menu window in window context
	//set pop menu -> the click with mouse
	PopupMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU2));
	//try init openGL in RenderWindow
	if (!raster.Init(RenderWindow)) return 0;
	//resize the GLwindow with the main window resize
	ResizeGLWindow(rect.right - rect.left, rect.bottom - rect.top);
	SetGLDefaults();
	if (!Window) {
		//logError("Failed to Create Window");
		return false;
	}

	while (true) {
		//render OpenGL
		Render();
		//messages of window main
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	// release the openGL
	raster.Release(RenderWindow);
	//return true of finalize program
	return true;
}

// Functions aux to WinMain
LRESULT CALLBACK MapDetailsDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Tratamento das mensagens eviadas pela janela
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		SetDlgItemText(hWnd, IDC_MAP_DETAILS_NAME, L"Map Name");
		
		//to get a item
		wchar_t temp[500];
		GetDlgItemText(hWnd, IDC_MAP_DETAILS_NAME, temp, 500);
		
		// List Box
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_RULES, LB_ADDSTRING, 0, (LPARAM)L"Erase Me");
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_RULES, LB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_RULES, LB_ADDSTRING, 0, (LPARAM)L"Exit");
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_RULES, LB_ADDSTRING, 0, (LPARAM)L"Get Fragged");
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_RULES, LB_SETCURSEL, 0, 1);

		// Combo Box
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Erase Me");
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_TYPE, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Single Player");
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Multi Player");
		SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_TYPE, CB_SETCURSEL, 0, 0);
	}break;
	case WM_COMMAND:
	{
		if (wParam == IDC_OK)
		{
			long level_rule = SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_RULES, LB_GETCURSEL, 0, 0);
			long level_type = SendDlgItemMessage(hWnd, IDC_MAP_DETAILS_LEVEL_TYPE, CB_GETCURSEL, 0, 0);

			wchar_t temp[500];
			swprintf_s(temp, 500, L"Level type: %i\r\nLevel Rule: %i\r\nOK Button!", level_type, level_rule);
			MessageBox(hWnd, temp, L"OK", MB_OK);

			EndDialog(hWnd, 0);
		}
		else if (wParam == IDC_CANCEL)
		{
			MessageBox(hWnd, L"Cancel Button!", L"Cancel", MB_OK);
			EndDialog(hWnd, 0);
		}
	}break;
	}

	return (0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Tratamento das mensagens eviadas pela janela
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		WMCommand(hWnd, msg, wParam, lParam);
		break;
	case WM_SIZE:
		WMSize(hWnd, msg, wParam, lParam);
		break;
	case WM_RBUTTONUP:
		DisplayPopupMenu(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	// Retorna o processamento de mensagem feita
	return (DefWindowProcW(hWnd, msg, wParam, lParam));

}

void WMCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (lParam == (LPARAM)bCreateWall)
		MessageBox(Window, L"You Pressed bCreateWall", L"Congrats!!!", MB_OK);
	else if (wParam == ID_FILE_EXIT) PostQuitMessage(0);
	else if (wParam == ID_DRAWING_WIREFRAME)
	{
		CheckMenuItem(Menu, ID_DRAWING_WIREFRAME, MF_CHECKED);
		CheckMenuItem(Menu, ID_DRAWING_SOLID, MF_UNCHECKED);
	}
	else if (wParam == ID_DRAWING_SOLID)
	{
		CheckMenuItem(Menu, ID_DRAWING_SOLID, MF_CHECKED);
		CheckMenuItem(Menu, ID_DRAWING_WIREFRAME, MF_UNCHECKED);
	}

	else if (wParam == ID_POPUP_MOVE)
		MessageBox(Window, L"Move", L"Click", MB_OK);
	else if (wParam == ID_POPUP_DELETE)
		MessageBox(Window, L"Delete", L"Click", MB_OK);
	else if (wParam == ID_POPUP_TEXTURE)
		MessageBox(Window, L"Texture", L"Click", MB_OK);
	else if (wParam == ID_POPUP_DUPLICATE)
		MessageBox(Window, L"Duplicate", L"Click", MB_OK);

	else if (wParam == ID_MAP_DETAILS) {
		DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_MAP_DETAILS), NULL, (DLGPROC)MapDetailsDlgProc);
	}

}

void WMSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	MoveWindow(RenderWindow,
		DEFAULT_BUTTON_WIDTH,
		0,
		rect.right - rect.left - DEFAULT_BUTTON_WIDTH,
		rect.bottom - rect.top, true);

	GetClientRect(hWnd, &rect);
	ResizeGLWindow(rect.right - rect.left, rect.bottom - rect.top);

}

void ResizeGLWindow(long width, long height) {

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-200, 200, -200, -200, -2000, 2000);
	glMatrixMode(GL_MODELVIEW);

}

void DisplayPopupMenu(long x, long y) {
	HMENU temp = GetSubMenu(PopupMenu, 0);
	POINT mouse;
	if (GetCursorPos(&mouse) == 0) return;

	TrackPopupMenu(temp, TPM_LEFTALIGN | TPM_RIGHTBUTTON, mouse.x, mouse.y, 0, Window, NULL);
}

// Functions GL -> OpenGL functions

void SetGLDefaults() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void Render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(0.05f, -0.05f, 0.0f);
	glVertex3f(0.95f, -0.05f, 0.0f);
	glVertex3f(0.95f, -0.95f, 0.0f);
	glVertex3f(0.05f, -0.95f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(-0.25f, -0.25f, 0.0f);
	glVertex3f(-0.50f, -0.125f, 0.0f);
	glVertex3f(-0.75f, -0.25f, 0.0f);
	glVertex3f(-0.875, -0.5f, 0.0f);
	glVertex3f(-0.75f, -0.75f, 0.0f);
	glEnd();




	glPopMatrix();

	SwapBuffers(raster.hDC);
}

