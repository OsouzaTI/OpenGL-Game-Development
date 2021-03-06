/*
	
	@autor: Osouza
	
	Start project:
		@date: 29/07/2020 
		@hour: 22:03:16  (GMT-4)

*/


#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

#include "Win32/RASTER.h"
#include "Map/MapFormat.h"

#include "resource.h"

#define DEFAULT_BUTTON_WIDTH	100
#define DEFAULT_BUTTON_HEIGHT	20

enum {
	CREATE_MODE_NULL = 0,
	CREATE_MODE_START,
	CREATE_MODE_SIZE,
	CREATE_MODE_FINISH,
};

enum {
	OBJECTTYPE_WALL = 1,
	OBJECTTYPE_FLOOR,
	OBJECTTYPE_CEILING,
	ITEM_START_POSITION,
	ITEM_DM_POSITION,
	ITEM_ENTITY,
	ITEM,
	SOUND,
	LIGHT
};

enum {
	DRAW_MODE_WIREFRAME = 0,
	DRAW_MODE_SOLID,
};

enum {
	MODE_TYPE_MOVE = 0,
};

/// <summary>
/// structure created for get and initialize 
/// the traduction of mouse coordenates of 2d
/// in third dimension.
/// </summary>
typedef struct
{

	long mouse_x;
	long mouse_y;
	double world_x;
	double world_y;
	double world_z;

} COORDS;

typedef struct
{

	long mode;
	long type;
	COORDS start;
	COORDS finish;

} CREATION_COORDS;

typedef struct
{

	bool draw_floor;
	bool draw_ceiling;
	bool draw_wall;
	bool draw_entity;
	bool draw_item;
	bool draw_sound;
	bool draw_light;
	

} LAYER;

typedef struct
{

	long draw_mode;

} CONFIG;

HINSTANCE		GlobalInstance;
HMENU			Menu;
HMENU			PopupMenu;
HWND			Window;
HWND			bCreateWall;
HWND			bCreateFloor;
HWND			bCreateCeiling;
HWND			RenderWindow;
HWND			bPlaceStartPosition;
HWND			bPlaceDMPosition;
HWND			bInsertEntity;
HWND			bInsertItem;
HWND			bInsertSound;
HWND			bInsertLight;
HWND			bSelectObject;
RASTER			raster;
CREATION_COORDS creation_coords;
MAP				*map = new MAP;
LAYER			layer;
CONFIG			config;
GLubyte			select_rgb[3];
//-- START TODOS --//
/*
	[]	comment the functions	(1/9)
	[*] comment the classes		(1/1)
	[]	comment the structs		(0/?)
	[] ...
*/
//-- END TODOS --//


// Prototypes functions

/// <summary>
///		this function is responsible for window proccess
/// </summary>
/// <param name="hWnd">the handle of window</param>
/// <param name="msg">the message</param>
/// <returns>this function return a LRESULT</returns>
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
///		this function is responsible for window popup proccess
/// </summary>
/// <param name="hWnd">the handle window popup</param>
/// <param name="msg">the message</param>
/// <returns>this function return a LRESULt</returns>
LRESULT CALLBACK MapDetailsDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
/// <summary>
///		this function compute the mouse coordenates in world tridimensional
/// </summary>
/// <param name="xPos">the mouse x</param>
/// <param name="yPos">the mouse y</param>
/// <returns>return a struct COORDS</returns>
COORDS ComputeMouseCoords(long xPos, long yPos);


// this group of functions properly handles the messages of
// window proccess (WndProc)
void WMCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WMSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WMLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WMLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WMMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



/// <summary>
/// this function is responsible for resizing the render window
/// </summary>
/// <param name="width">the width of window</param>
/// <param name="height">the hight of window</param>
void ResizeGLWindow(long width, long height);

/// <summary>
/// this function make a popup menu in the window
/// </summary>
/// <param name="x">the mouse x</param>
/// <param name="y">the mouse y</param>
void DisplayPopupMenu(long x, long y);

void ShowSelectedButton();

/// <summary>
///	this function set de OpenGL with defaults config
/// </summary>
void SetGLDefaults();

/// <summary>
/// the openGL render
/// </summary>
void Render();
void Move();
void SelectAnObject();
void DrawWireframe();
void DrawSolid();
void DrawStartPosition();
void DrawDeathMatchPositions();
void DrawEntities(bool want_srgb = false);
void DrawItem(bool want_srgb = false);
void DrawSound(bool want_srgb = false);
void DrawLight(bool want_srgb = false);
void DrawPointMouse();

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

	//ShowCursor(false);

	//this function return a bool, is success the bool is true and
	//the second parameter of type RECT recieve the object
	//GetClientRect(Window, &rect); BUG FILHO DA PUTA
	GetWindowRect(Window, &rect);

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
	bCreateFloor = CreateWindow(
		L"BUTTON",
		L"Create Floor",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 2,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);
	bCreateCeiling = CreateWindow(
		L"BUTTON",
		L"Create Ceiling",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 4,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);

	bPlaceStartPosition = CreateWindow(
		L"BUTTON",
		L"Place Start",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 6,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);

	bPlaceDMPosition = CreateWindow(
		L"BUTTON",
		L"Place DM",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 8,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);

	bInsertEntity = CreateWindow(
		L"BUTTON",
		L"Insert Entity",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 10,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);
	
	bInsertItem = CreateWindow(
		L"BUTTON",
		L"Insert Item",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 12,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);

	bInsertSound = CreateWindow(
		L"BUTTON",
		L"Insert Sound",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 14,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);
	
	bInsertLight = CreateWindow(
		L"BUTTON",
		L"Insert Light",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 16,
		DEFAULT_BUTTON_WIDTH,
		DEFAULT_BUTTON_HEIGHT,
		Window,
		NULL,
		hInstance, NULL);
	
	bSelectObject = CreateWindow(
		L"BUTTON",
		L"Select Object",
		WS_CHILD | WS_VISIBLE,
		0, 100 + DEFAULT_BUTTON_HEIGHT * 18,
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

	if (GetDeviceCaps(raster.hDC, BITSPIXEL) < 24)
		MessageBox(Window, L"This software not work in depth pixel < 24", L"Aviso", MB_OK);

	//resize the GLwindow with the main window resize
	ResizeGLWindow(rect.right - rect.left, rect.bottom - rect.top);
	SetGLDefaults();

	memset(&creation_coords, 0, sizeof(creation_coords));
	memset(&layer,  1, sizeof(layer));
	memset(&config, 0, sizeof(config));

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
LRESULT CALLBACK InsertLightDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[500];

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_R, L"255");
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_G, L"255");
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_B, L"255");
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_XA, L"0");
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_YA, L"0");
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_ZA, L"0");

		swprintf_s(temp, 500, L"%f", creation_coords.start.world_x);
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_X, temp);
		
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_Y, L"0.0");
		
		swprintf_s(temp, 500, L"%f", creation_coords.start.world_z);
		SetDlgItemText(hWnd, IDC_INSERT_LIGHT_Z, temp);

	}break;
	case WM_COMMAND:
	{
		if (wParam == IDCANCEL) EndDialog(hWnd, 0);
		else if (wParam == IDOK)
		{

			wchar_t temp[500];

			GLfloat r;
			GLfloat g;
			GLfloat b;
			GLfloat xa;
			GLfloat ya;
			GLfloat za;
			GLfloat x;
			GLfloat y;
			GLfloat z;
			
			//RGB
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_R, temp, 500);
			swscanf_s(temp, L"%f", &r);
			r = r / 255.0f;
			
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_G, temp, 500);
			swscanf_s(temp, L"%f", &g);
			g = g / 255.0f;
			
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_B, temp, 500);
			swscanf_s(temp, L"%f", &b);
			b = b / 255.0f;
			//XA
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_XA, temp, 500);
			swscanf_s(temp, L"%f", &xa);
			
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_YA, temp, 500);
			swscanf_s(temp, L"%f", &ya);
			
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_ZA, temp, 500);
			swscanf_s(temp, L"%f", &za);

			//X
			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_X, temp, 500);
			swscanf_s(temp, L"%f", &x);

			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_Y, temp, 500);
			swscanf_s(temp, L"%f", &y);

			GetDlgItemText(hWnd, IDC_INSERT_LIGHT_Z, temp, 500);
			swscanf_s(temp, L"%f", &z);

			swprintf_s(temp, 500, L"Light #%i", map->header.max_lights);
			map->InsertLight((char*)temp, (char*)"lightmap.bmp", x, y, z, xa, ya, za, r, g, b);

			EndDialog(hWnd, 1);
		}
	}break;
	default:
		break;
	}
	
	return (0);
}

LRESULT CALLBACK InsertSoundDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		SetDlgItemText(hWnd, IDC_INSERTSOUND_FILENAME, L"my_sound.wav");
	}break;
	case WM_COMMAND:
	{
		if (wParam == IDCANCEL) EndDialog(hWnd, 0);
		else if (wParam == IDOK)
		{
			
			char temp[500];
			GetDlgItemText(hWnd, IDC_INSERTSOUND_FILENAME, (LPWSTR)temp, 500);			
			
			wchar_t teste[500];
			

			bool b = map->InsertSound(
				creation_coords.start.world_x,
				creation_coords.start.world_y,
				creation_coords.start.world_z,
				temp
			);

			swprintf_s(teste, 500, L"%i", b);
			//MessageBox(hWnd, teste, L"Sounds", MB_OK);

			EndDialog(hWnd, 1);

		}
	}break;
	default:
		break;
	}
	
	return (0);
}

LRESULT CALLBACK InsertItemDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		SendDlgItemMessage(hWnd, IDC_INSERT_ITEM_TYPE, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hWnd, IDC_INSERT_ITEM_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Gun");
		SendDlgItemMessage(hWnd, IDC_INSERT_ITEM_TYPE, CB_SETCURSEL, 0, 0);
		SetDlgItemText(hWnd, IDC_INSERT_ITEM_RESPAWN_TIME, L"0");
		SetDlgItemText(hWnd, IDC_INSERT_ITEM_RESPAWN_WAIT, L"0");
	}break;
	case WM_COMMAND:
	{
		if (wParam == IDCANCEL) EndDialog(hWnd, 0);
		else if (wParam == IDOK)
		{
			
			wchar_t temp[500];
			long type;
			long respawn_time;
			long respawn_wait;

			type = SendDlgItemMessage(hWnd, IDC_INSERT_ITEM_TYPE, CB_GETCURSEL, 0, 0);
			GetDlgItemText(hWnd, IDC_INSERT_ITEM_RESPAWN_TIME, temp, 500);
			swscanf_s(temp, L"%i", &respawn_time);
			GetDlgItemText(hWnd, IDC_INSERT_ITEM_RESPAWN_WAIT, temp, 500);
			swscanf_s(temp, L"%i", &respawn_wait);

			map->InsertItem(
				creation_coords.start.world_x,
				creation_coords.start.world_y,
				creation_coords.start.world_z,
				type,
				respawn_wait,
				respawn_time
			);

			EndDialog(hWnd, 1);

		}
	}break;
	default:
		break;
	}
	
	return (0);
}

LRESULT CALLBACK InsertEntityDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		SendDlgItemMessage(hWnd, IDC_INSERT_ENTITY_TYPE, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hWnd, IDC_INSERT_ENTITY_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Joe");
		SetDlgItemText(hWnd, IDC_INSERT_ENTITY_HEALTH,		L"100");
		SetDlgItemText(hWnd, IDC_INSERT_ENTITY_STRENGHT,	L"10");
		SetDlgItemText(hWnd, IDC_INSERT_ENTITY_ARMOUR,		L"0");
	}break;
	case WM_COMMAND:
	{
		if (wParam == IDCANCEL) EndDialog(hWnd, 0);
		else if (wParam == IDOK)
		{

			long type;
			long health;
			long strenght;
			long armour;
			wchar_t temp[500];

			type = SendDlgItemMessage(hWnd, IDC_INSERT_ENTITY_TYPE, CB_GETCURSEL, 0, 0);
			
			GetDlgItemText(hWnd, IDC_INSERT_ENTITY_HEALTH, temp, 500);			
			swscanf_s(temp, L"%i", &health);
			GetDlgItemText(hWnd, IDC_INSERT_ENTITY_STRENGHT, temp, 500);			
			swscanf_s(temp, L"%i", &strenght);
			GetDlgItemText(hWnd, IDC_INSERT_ENTITY_ARMOUR, temp, 500);			
			swscanf_s(temp, L"%i", &armour);

			
			map->InsertEntity(
				type,
				creation_coords.start.world_x,
				creation_coords.start.world_y,
				creation_coords.start.world_z,
				0,0,0, health, strenght, armour
			);

			EndDialog(hWnd, 1);
		}
	}break;
	default:
		break;
	}
	
	return (0);
}

LRESULT CALLBACK DMPositionDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		SendDlgItemMessage(hWnd, IDC_DM_POSITION_TYPE, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hWnd, IDC_DM_POSITION_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Player 1");
		SendDlgItemMessage(hWnd, IDC_DM_POSITION_TYPE, CB_ADDSTRING, 0, (LPARAM)L"Player 2");
		SendDlgItemMessage(hWnd, IDC_DM_POSITION_TYPE, CB_SETCURSEL, 0, 1);
	}break;
	case WM_COMMAND:
	{
		if (wParam == IDCANCEL) EndDialog(hWnd, 0);
		else if (wParam == IDOK)
		{

			long player = SendDlgItemMessage(hWnd, IDC_DM_POSITION_TYPE, CB_GETCURSEL, 0, 0);
			
			map->details.deathmatch[player].xyz[0] = creation_coords.start.world_x;
			map->details.deathmatch[player].xyz[1] = creation_coords.start.world_y;
			map->details.deathmatch[player].xyz[2] = creation_coords.start.world_z;
			
			EndDialog(hWnd, 1);

		}
	}break;
	default:
		break;
	}
	
	return (0);
}

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
		case WM_LBUTTONDOWN:
			WMLButtonDown(hWnd, msg, wParam, lParam);
			break;
		case WM_LBUTTONUP:
			WMLButtonUp(hWnd, msg, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			WMMouseMove(hWnd, msg, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// Retorna o processamento de mensagem feita
	return 0;

}

COORDS ComputeMouseCoords(long xPos, long yPos)
{

	COORDS coords;
	RECT rect;

	float window_width;
	float window_height;

	float window_start_x;
	float window_start_y;

	coords.mouse_x = xPos ;
	coords.mouse_y = yPos ;

	GetWindowRect(RenderWindow, &rect);

	window_width	= (float)(rect.right - rect.left);
	window_height	= (float)(rect.bottom - rect.top);
	window_start_x	= (float)(coords.mouse_x - rect.left);
	window_start_y	= (float)(coords.mouse_y);
	
	coords.world_x =   ((window_start_x / window_width)  * 2.0) - 1.0;
	coords.world_z =  -(((window_start_y / window_height) * 2.0) - 1.0);

	return coords;

}

void WMCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (lParam == (LPARAM)bCreateWall) {		
		creation_coords.type = OBJECTTYPE_WALL;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bCreateFloor) {		
		creation_coords.type = OBJECTTYPE_FLOOR;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bCreateCeiling) {		
		creation_coords.type = OBJECTTYPE_CEILING;	
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bPlaceStartPosition) {			
		creation_coords.type = ITEM_START_POSITION;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bPlaceDMPosition) {			
		creation_coords.type = ITEM_DM_POSITION;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bInsertEntity) {			
		creation_coords.type = ITEM_ENTITY;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bInsertItem) {			
		creation_coords.type = ITEM;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bInsertSound) {			
		creation_coords.type = SOUND;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bInsertLight) {			
		creation_coords.type = LIGHT;
		ShowSelectedButton();
	}
	else if (lParam == (LPARAM)bSelectObject) {			
		creation_coords.type = NULL;
		ShowSelectedButton();
	}	
	else if (wParam == ID_FILE_EXIT) PostQuitMessage(0);
	else if (wParam == ID_DRAWING_WIREFRAME)
	{
		CheckMenuItem(Menu, ID_DRAWING_WIREFRAME, MF_CHECKED);
		CheckMenuItem(Menu, ID_DRAWING_SOLID, MF_UNCHECKED);

		config.draw_mode = DRAW_MODE_WIREFRAME;

	}
	else if (wParam == ID_DRAWING_SOLID)
	{
		CheckMenuItem(Menu, ID_DRAWING_SOLID, MF_CHECKED);
		CheckMenuItem(Menu, ID_DRAWING_WIREFRAME, MF_UNCHECKED);

		config.draw_mode = DRAW_MODE_SOLID;

	}
	else if (wParam == ID_LAYERS_FLOOR)
	{
		if (layer.draw_floor) {
			CheckMenuItem(Menu, ID_LAYERS_FLOOR, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_FLOOR, MF_CHECKED);
		}
		layer.draw_floor = !layer.draw_floor;
	}
	else if (wParam == ID_LAYERS_CEILING)
	{
		if (layer.draw_ceiling) {
			CheckMenuItem(Menu, ID_LAYERS_CEILING, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_CEILING, MF_CHECKED);
		}
		layer.draw_ceiling = !layer.draw_ceiling;
	}
	else if (wParam == ID_LAYERS_WALL)
	{
		if (layer.draw_wall) {
			CheckMenuItem(Menu, ID_LAYERS_WALL, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_WALL, MF_CHECKED);
		}
		layer.draw_wall = !layer.draw_wall;
	}
	else if (wParam == ID_LAYERS_ENTITY)
	{
		if (layer.draw_entity) {
			CheckMenuItem(Menu, ID_LAYERS_ENTITY, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_ENTITY, MF_CHECKED);
		}
		layer.draw_entity = !layer.draw_entity;
	}
	else if (wParam == ID_LAYERS_ITEM)
	{
		if (layer.draw_item) {
			CheckMenuItem(Menu, ID_LAYERS_ITEM, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_ITEM, MF_CHECKED);
		}
		layer.draw_item = !layer.draw_item;
	}
	else if (wParam == ID_LAYERS_SOUND)
	{
		if (layer.draw_sound) {
			CheckMenuItem(Menu, ID_LAYERS_SOUND, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_SOUND, MF_CHECKED);
		}
		layer.draw_sound = !layer.draw_sound;
	}
	else if (wParam == ID_LAYERS_LIGHT)
	{
		if (layer.draw_light) {
			CheckMenuItem(Menu, ID_LAYERS_LIGHT, MF_UNCHECKED);
		}
		else {
			CheckMenuItem(Menu, ID_LAYERS_LIGHT, MF_CHECKED);
		}
		layer.draw_light = !layer.draw_light;
	}
	else if (wParam == ID_POPUP_MOVE)
	{
		//MessageBox(hWnd, L"OPAAA", L"Erro", MB_OK);
		if (select_rgb[0] != 255 && select_rgb[1] != 255 && select_rgb[2] != 255)
		{
			creation_coords.start = creation_coords.finish;
			creation_coords.mode = CREATE_MODE_START;
			creation_coords.type = MODE_TYPE_MOVE;
		}
		else {
			MessageBox(hWnd, L"Um objeto deve ser selecionado", L"Erro", MB_OK);
		}
	}
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

void WMLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	creation_coords.mode   = CREATE_MODE_START;
	creation_coords.start  = ComputeMouseCoords(LOWORD(lParam), HIWORD(lParam));
	creation_coords.finish = creation_coords.start;
}

void WMLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (creation_coords.mode != CREATE_MODE_NULL)
	{
		creation_coords.mode = CREATE_MODE_NULL;
		creation_coords.finish = ComputeMouseCoords(LOWORD(lParam), HIWORD(lParam));
		
		if (creation_coords.type == OBJECTTYPE_WALL)
		{
			char name[] = "Wall";
			map->InsertObject(name, creation_coords.type);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.start.world_x,
				1,
				creation_coords.start.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.finish.world_x,
				1,
				creation_coords.finish.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.finish.world_x,
				0,
				creation_coords.finish.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.start.world_x,
				0,
				creation_coords.start.world_z
			);

			map->InsertTriangle(
				map->header.max_objects - 1,
				0, 1, 2,
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f
			);
			
			map->InsertTriangle(
				map->header.max_objects - 1,
				2, 3, 0,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f
			);

		}
		else if (creation_coords.type == OBJECTTYPE_FLOOR)
		{
			char name[] = "Floor";
			map->InsertObject(name, creation_coords.type);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.start.world_x,
				0,
				creation_coords.start.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.finish.world_x,
				0,
				creation_coords.start.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.finish.world_x,
				0,
				creation_coords.finish.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.start.world_x,
				0,
				creation_coords.finish.world_z
			);

			map->InsertTriangle(
				map->header.max_objects - 1,
				0, 1, 2,
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f
			);

			map->InsertTriangle(
				map->header.max_objects - 1,
				2, 3, 0,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f
			);

		}
		else if(creation_coords.type == OBJECTTYPE_CEILING)
		{
			char name[] = "Ceiling";
			map->InsertObject(name, creation_coords.type);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.start.world_x,
				1,
				creation_coords.start.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.finish.world_x,
				1,
				creation_coords.start.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.finish.world_x,
				1,
				creation_coords.finish.world_z
			);
			map->InsertVertex(
				map->header.max_objects - 1,
				creation_coords.start.world_x,
				1,
				creation_coords.finish.world_z
			);

			map->InsertTriangle(
				map->header.max_objects - 1,
				0, 1, 2,
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f
			);

			map->InsertTriangle(
				map->header.max_objects - 1,
				2, 3, 0,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f
			);

		}
		else if (creation_coords.type == ITEM_START_POSITION)
		{

			long rgb = map->GenerateColor();
			map->details.single_player.select_rgb[0] = GetRValue(rgb);
			map->details.single_player.select_rgb[1] = GetGValue(rgb);
			map->details.single_player.select_rgb[2] = GetBValue(rgb);

			map->details.single_player.xyz[0] = creation_coords.start.world_x;
			map->details.single_player.xyz[1] = creation_coords.start.world_y;
			map->details.single_player.xyz[2] = creation_coords.start.world_z;

		}
		else if (creation_coords.type == ITEM_DM_POSITION) {

			DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_DM_POSITION), NULL, (DLGPROC)DMPositionDlgProc);

		}
		else if (creation_coords.type == ITEM_ENTITY)
		{
	
			DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_INSERT_ENTITY), NULL, (DLGPROC)InsertEntityDlgProc);

		}
		else if (creation_coords.type == ITEM)
		{
	
			DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_INSERT_ITEM), NULL, (DLGPROC)InsertItemDlgProc);

		}
		else if (creation_coords.type == SOUND)
		{
	
			DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_INSERT_SOUND), NULL, (DLGPROC)InsertSoundDlgProc);

		}
		else if (creation_coords.type == LIGHT)
		{
	
			DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_INSERT_LIGHT), NULL, (DLGPROC)InsertLightDlgProc);

		}
		else if (creation_coords.type == NULL)
		{	
			SelectAnObject();
		}
		
		memset(&creation_coords.start, 0, sizeof(creation_coords.start));
		memset(&creation_coords.finish, 0, sizeof(creation_coords.finish));

	}
	else if (creation_coords.type == NULL) {
	
		SelectAnObject();

	}

}

void WMMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	wchar_t temp[500];
	
	//wchar_t buffer[100];
	//swprintf_s(buffer, 100, L"%i", sizeof(wchar_t));
	//MessageBox(Window, L"Move", buffer, MB_OK);
	if (creation_coords.mode != CREATE_MODE_NULL)
	{

		if (creation_coords.type == MODE_TYPE_MOVE)
		{
			Move();
		}

		creation_coords.mode = CREATE_MODE_SIZE;
		creation_coords.finish = ComputeMouseCoords(LOWORD(lParam), HIWORD(lParam));
		
		swprintf_s(
			temp,
			500,
			L"Map Editor, Mx=%i My=%i, X=%0.4f Z=%0.4f | (R=%i,G=%i,B=%i)",
			creation_coords.finish.mouse_x, 
			creation_coords.finish.mouse_y,
			creation_coords.finish.world_x,
			creation_coords.finish.world_z,
			select_rgb[0],
			select_rgb[1],
			select_rgb[2]
		);
	
	}
	else {
		
		swprintf_s(
			temp,
			500,
			L"Map Editor Move Mode, Mx=%i My=%i",
			LOWORD(lParam), HIWORD(lParam)
		);
		
	}

	SetWindowText(Window, temp);

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

void ShowSelectedButton()
{
	SetWindowText(bCreateWall,			L"Create Wall");
	SetWindowText(bCreateCeiling,		L"Create Ceiling");
	SetWindowText(bCreateFloor,			L"Create Floor");
	SetWindowText(bPlaceStartPosition,	L"Place Start");
	SetWindowText(bPlaceDMPosition,		L"Place DM");
	SetWindowText(bInsertEntity,		L"Insert Entity");
	SetWindowText(bInsertItem,		L"Insert Item");
	SetWindowText(bInsertSound,		L"Insert Sound");
	SetWindowText(bInsertLight,		L"Insert Light");

	switch (creation_coords.type)
	{
		case OBJECTTYPE_FLOOR:
			SetWindowText(bCreateFloor, L"* Floor *");
			break;
		case OBJECTTYPE_WALL:
			SetWindowText(bCreateWall, L"* Wall *");
			break;
		case OBJECTTYPE_CEILING:
			SetWindowText(bCreateCeiling, L"* Ceiling *");
			break;	
		case ITEM_START_POSITION:
			SetWindowText(bPlaceStartPosition, L"* StartPos *");
			break;
		case ITEM_DM_POSITION:
			SetWindowText(bPlaceDMPosition, L"* DM Pos *");
			break;
		case ITEM_ENTITY:
			SetWindowText(bInsertEntity, L"* Insert Entity *");
			break;
		case ITEM:
			SetWindowText(bInsertItem, L"* Insert Item *");
			break;
		case SOUND:
			SetWindowText(bInsertSound, L"* Insert Sound *");
			break;
		case LIGHT:
			SetWindowText(bInsertLight, L"* Insert Light *");
			break;
		default:
			SetWindowText(bSelectObject, L"* Select Object *");
			break;
	}

}

void SelectAnObject()
{

	GLubyte rgb[3];
	RECT rect;
	long x;
	long y;
	long width;
	long height;
	bool found = false;

	memset(&select_rgb, 0xFF, sizeof(select_rgb));
	GetClientRect(RenderWindow, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	x = creation_coords.finish.mouse_x - DEFAULT_BUTTON_WIDTH;
	y = height - creation_coords.finish.mouse_y;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLineWidth(4.0);
	DrawSolid();
	if (layer.draw_entity) DrawEntities(true);
	if (layer.draw_item) DrawItem(true);
	glLineWidth(1.0);

	glReadBuffer(GL_BACK);
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rgb);

	for (long i = 0; i < map->header.max_objects; i++)
	{
		if (
			(map->object[i].select_rgb[0] == rgb[0]) &&
			(map->object[i].select_rgb[1] == rgb[1]) &&
			(map->object[i].select_rgb[2] == rgb[2])
			) {
			found = true;			
		}
	}

	for (long i = 0; i < map->header.max_entities; i++)
	{
		if (
			(map->entity[i].select_rgb[0] == rgb[0]) &&
			(map->entity[i].select_rgb[1] == rgb[1]) &&
			(map->entity[i].select_rgb[2] == rgb[2])
			) {
			found = true;			
		}
	}

	for (long i = 0; i < map->header.max_items; i++)
	{
		if (
			(map->item[i].select_rgb[0] == rgb[0]) &&
			(map->item[i].select_rgb[1] == rgb[1]) &&
			(map->item[i].select_rgb[2] == rgb[2])
			) {
			found = true;			
		}
	}

	for (long i = 0; i < map->header.max_lights; i++)
	{
		if (
			(map->light[i].select_rgb[0] == rgb[0]) &&
			(map->light[i].select_rgb[1] == rgb[1]) &&
			(map->light[i].select_rgb[2] == rgb[2])
			) {
			found = true;			
		}
	}

	for (long i = 0; i < map->header.max_sounds; i++)
	{
		if (
			(map->sound[i].select_rgb[0] == rgb[0]) &&
			(map->sound[i].select_rgb[1] == rgb[1]) &&
			(map->sound[i].select_rgb[2] == rgb[2])
			) {
			found = true;			
		}
	}

	if (
		(map->details.single_player.select_rgb[0] == rgb[0]) &&
		(map->details.single_player.select_rgb[1] == rgb[1]) &&
		(map->details.single_player.select_rgb[2] == rgb[2]) ||

		(map->details.deathmatch[0].select_rgb[0] == rgb[0]) &&
		(map->details.deathmatch[0].select_rgb[1] == rgb[1]) &&
		(map->details.deathmatch[0].select_rgb[2] == rgb[2]) ||

		(map->details.deathmatch[1].select_rgb[0] == rgb[0]) &&
		(map->details.deathmatch[1].select_rgb[1] == rgb[1]) &&
		(map->details.deathmatch[1].select_rgb[2] == rgb[2])
	)
	{
		found = true;		
	}

	if (found) {		
		select_rgb[0] = rgb[0];
		select_rgb[1] = rgb[1];
		select_rgb[2] = rgb[2];
	}

}

void Move()
{
	float x;
	float z;

	x = (float)(creation_coords.finish.world_x - creation_coords.start.world_x);
	z = (float)(creation_coords.finish.world_z - creation_coords.start.world_z);

	creation_coords.start = creation_coords.finish;

	if (
		map->details.single_player.select_rgb[0] == select_rgb[0] &&
		map->details.single_player.select_rgb[1] == select_rgb[1] &&
		map->details.single_player.select_rgb[2] == select_rgb[2]
		) {
		map->details.single_player.xyz[0] += x;
		map->details.single_player.xyz[1] += z;
		return;
	}

	for (long i = 0; i < 2; i++)
	{
		if (
			map->details.deathmatch[i].select_rgb[0] == select_rgb[0] &&
			map->details.deathmatch[i].select_rgb[1] == select_rgb[1] &&
			map->details.deathmatch[i].select_rgb[2] == select_rgb[2]
			) {
			map->details.deathmatch[i].xyz[0] += x;
			map->details.deathmatch[i].xyz[2] += z;
			return;
		}
	}

	for (long i = 0; i < map->header.max_objects; i++)
	{
		if (
			map->object[i].select_rgb[0] == select_rgb[0] &&
			map->object[i].select_rgb[1] == select_rgb[1] &&
			map->object[i].select_rgb[2] == select_rgb[2]
			) {
			for (long j = 0; j < map->object[i].max_vertices; j++)
			{
				map->object[i].vertex[j].xyz[0] += x;
				map->object[i].vertex[j].xyz[2] += z;
			}
			return;
		}
	}

	for (long i = 0; i < map->header.max_entities; i++)
	{
		if (
			map->entity[i].select_rgb[0] == select_rgb[0] &&
			map->entity[i].select_rgb[1] == select_rgb[1] &&
			map->entity[i].select_rgb[2] == select_rgb[2]
			) {
			
			map->entity[i].xyz[0] += x;
			map->entity[i].xyz[2] += z;

			return;
		}
	}

	for (long i = 0; i < map->header.max_items; i++)
	{
		if (
			map->item[i].select_rgb[0] == select_rgb[0] &&
			map->item[i].select_rgb[1] == select_rgb[1] &&
			map->item[i].select_rgb[2] == select_rgb[2]
			) {
			
			map->item[i].xyz[0] += x;
			map->item[i].xyz[2] += z;

			return;
		}
	}

	for (long i = 0; i < map->header.max_sounds; i++)
	{
		if (
			map->sound[i].select_rgb[0] == select_rgb[0] &&
			map->sound[i].select_rgb[1] == select_rgb[1] &&
			map->sound[i].select_rgb[2] == select_rgb[2]
			) {
			
			map->sound[i].xyz[0] += x;
			map->sound[i].xyz[2] += z;

			return;
		}
	}

	for (long i = 0; i < map->header.max_lights; i++)
	{
		if (
			map->light[i].select_rgb[0] == select_rgb[0] &&
			map->light[i].select_rgb[1] == select_rgb[1] &&
			map->light[i].select_rgb[2] == select_rgb[2]
			) {
			
			map->light[i].xyz[0] += x;
			map->light[i].xyz[2] += z;

			return;
		}
	}

}

// Functions GL -> OpenGL functions

void SetGLDefaults() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);

		DrawStartPosition();
		DrawDeathMatchPositions();
		DrawEntities();
		DrawItem();
		DrawSound();
		DrawLight();
		DrawPointMouse();

		if (config.draw_mode == DRAW_MODE_WIREFRAME) {
			DrawWireframe();
		}
		else if (config.draw_mode == DRAW_MODE_SOLID) {
			DrawSolid();
		}

		if (creation_coords.type == OBJECTTYPE_WALL)
		{
			glBegin(GL_LINES);
			glVertex2d(
				creation_coords.start.world_x,
				creation_coords.start.world_z
			);
			glVertex2d(
				creation_coords.finish.world_x,
				creation_coords.finish.world_z
			);
			glEnd();
		}
		else if(
			creation_coords.type == OBJECTTYPE_FLOOR ||
			creation_coords.type == OBJECTTYPE_CEILING 
		)
		{

			glBegin(GL_LINE_LOOP);
			glVertex2d(
				creation_coords.start.world_x,
				creation_coords.start.world_z
			);
			glVertex2d(
				creation_coords.finish.world_x,
				creation_coords.start.world_z
			);
			glVertex2d(
				creation_coords.finish.world_x,
				creation_coords.finish.world_z
			);
			glVertex2d(
				creation_coords.start.world_x,
				creation_coords.finish.world_z
			);
			glEnd();
		}

	glPopMatrix();
	SwapBuffers(raster.hDC);
}

void DrawWireframe()
{
	if (map->header.max_objects > 0)
	{
		for (long i = 0; i < map->header.max_objects; i++)
		{
			if (
				(layer.draw_floor   && map->object[i].type == OBJECTTYPE_FLOOR) ||
				(layer.draw_ceiling && map->object[i].type == OBJECTTYPE_CEILING) ||
				(layer.draw_wall    && map->object[i].type == OBJECTTYPE_WALL)
			)
			{

				glBegin(GL_LINE_LOOP);
				for (long j = 0; j < map->object[i].max_vertices; j++)
				{
					glVertex2d(
						map->object[i].vertex[j].xyz[0],
						map->object[i].vertex[j].xyz[2]
					);
				}
				glEnd();
			}
		}
	}
}

void DrawSolid()
{
	for (long i = 0; i < map->header.max_objects; i++)
	{
		glColor3ubv(map->object[i].select_rgb);

		if (
			(layer.draw_floor && map->object[i].type == OBJECTTYPE_FLOOR) ||
			(layer.draw_ceiling && map->object[i].type == OBJECTTYPE_CEILING)
		)
		{

			glBegin(GL_TRIANGLES);
			for (long j = 0; j < map->object[i].max_triangles; j++)
			{
				long vertex_0 = map->object[i].triangle[j].point[0];
				long vertex_1 = map->object[i].triangle[j].point[1];
				long vertex_2 = map->object[i].triangle[j].point[2];

				// Pela visao 2d usamos esse render
				glVertex2d(
					map->object[i].vertex[vertex_0].xyz[0],					
					map->object[i].vertex[vertex_0].xyz[2]
				);
				glVertex2d(
					map->object[i].vertex[vertex_1].xyz[0],					
					map->object[i].vertex[vertex_1].xyz[2]
				);
				glVertex2d(
					map->object[i].vertex[vertex_2].xyz[0],					
					map->object[i].vertex[vertex_2].xyz[2]
				);
			}
			glEnd();
		}
		else if(layer.draw_wall && map->object[i].type == OBJECTTYPE_WALL)
		{

			for (long j = 0; j < map->object[i].max_triangles; j++)
			{

				long vertex_0 = map->object[i].triangle[j].point[0];
				long vertex_1 = map->object[i].triangle[j].point[1];
				long vertex_2 = map->object[i].triangle[j].point[2];

				glBegin(GL_LINE_LOOP);

					glVertex2d(
						map->object[i].vertex[vertex_0].xyz[0],
						map->object[i].vertex[vertex_0].xyz[2]
					);
					glVertex2d(
						map->object[i].vertex[vertex_1].xyz[0],
						map->object[i].vertex[vertex_1].xyz[2]
					);
					glVertex2d(
						map->object[i].vertex[vertex_2].xyz[0],
						map->object[i].vertex[vertex_2].xyz[2]
					);

				glEnd();

			}

		}

	}
}

void DrawStartPosition()
{
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2d(
			map->details.single_player.xyz[0],
			map->details.single_player.xyz[2]-0.01f
		);
		glVertex2d(
			map->details.single_player.xyz[0]+0.01f,
			map->details.single_player.xyz[2]
		);
		glVertex2d(
			map->details.single_player.xyz[0],
			map->details.single_player.xyz[2] + 0.01f
		);
		glVertex2d(
			map->details.single_player.xyz[0] - 0.01f,
			map->details.single_player.xyz[2]
		);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	/// PAgina 168 livro traduzido

}

void DrawDeathMatchPositions()
{
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2d(
			map->details.deathmatch[0].xyz[0],
			map->details.deathmatch[0].xyz[2]-0.01f
		);
		glVertex2d(
			map->details.deathmatch[0].xyz[0]+0.01f,
			map->details.deathmatch[0].xyz[2]
		);
		glVertex2d(
			map->details.deathmatch[0].xyz[0],
			map->details.deathmatch[0].xyz[2] + 0.01f
		);
		glVertex2d(
			map->details.deathmatch[0].xyz[0] - 0.01f,
			map->details.deathmatch[0].xyz[2]
		);

		glVertex2d(
			map->details.deathmatch[1].xyz[0],
			map->details.deathmatch[1].xyz[2]-0.01f
		);
		glVertex2d(
			map->details.deathmatch[1].xyz[0]+0.01f,
			map->details.deathmatch[1].xyz[2]
		);
		glVertex2d(
			map->details.deathmatch[1].xyz[0],
			map->details.deathmatch[1].xyz[2] + 0.01f
		);
		glVertex2d(
			map->details.deathmatch[1].xyz[0] - 0.01f,
			map->details.deathmatch[1].xyz[2]
		);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	/// PAgina 168 livro traduzido

}

void DrawEntities(bool want_srgb)
{
	if (layer.draw_entity)
	{
		glColor3f(0.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);

		for (long i = 0; i < map->header.max_entities; i++)
		{

			if (
				(map->entity[i].select_rgb[0] == select_rgb[0]) &&
				(map->entity[i].select_rgb[1] == select_rgb[1]) &&
				(map->entity[i].select_rgb[2] == select_rgb[2])
				)
			{
				glColor3f(1.0f, 0.0f, 0.0f);
			}

			glVertex2d(
				map->entity[i].xyz[0],
				map->entity[i].xyz[2] - 0.01f
			);
			glVertex2d(
				map->entity[i].xyz[0] + 0.01f,
				map->entity[i].xyz[2]
			);
			glVertex2d(
				map->entity[i].xyz[0],
				map->entity[i].xyz[2] + 0.01
			);
			glVertex2d(
				map->entity[i].xyz[0] - 0.01f,
				map->entity[i].xyz[2]
			);

		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
}

void DrawItem(bool want_srgb) {

	if (layer.draw_item)
	{
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_QUADS);

		for (long i = 0; i < map->header.max_items; i++)
		{

			glVertex2d(
				map->item[i].xyz[0],
				map->item[i].xyz[2] - 0.01f
			);
			glVertex2d(
				map->item[i].xyz[0] + 0.01f,
				map->item[i].xyz[2]
			);
			glVertex2d(
				map->item[i].xyz[0],
				map->item[i].xyz[2] + 0.01
			);
			glVertex2d(
				map->item[i].xyz[0] - 0.01f,
				map->item[i].xyz[2]
			);

		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
}

void DrawSound(bool want_srgb)
{
	if (layer.draw_sound) {
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_QUADS);

		for (long i = 0; i < map->header.max_sounds; i++)
		{

			glVertex2d(
				map->sound[i].xyz[0],
				map->sound[i].xyz[2] - 0.01f
			);
			glVertex2d(
				map->sound[i].xyz[0] + 0.01f,
				map->sound[i].xyz[2]
			);
			glVertex2d(
				map->sound[i].xyz[0],
				map->sound[i].xyz[2] + 0.01
			);
			glVertex2d(
				map->sound[i].xyz[0] - 0.01f,
				map->sound[i].xyz[2]
			);

		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
}

void DrawLight(bool want_srgb)
{

	if (layer.draw_light) {
		glColor3f(0.5f, 1.0f, 0.0f);
		glBegin(GL_QUADS);

		for (long i = 0; i < map->header.max_lights; i++)
		{

			glVertex2d(
				map->light[i].xyz[0],
				map->light[i].xyz[2] - 0.01f
			);
			glVertex2d(
				map->light[i].xyz[0] + 0.01f,
				map->light[i].xyz[2]
			);
			glVertex2d(
				map->light[i].xyz[0],
				map->light[i].xyz[2] + 0.01
			);
			glVertex2d(
				map->light[i].xyz[0] - 0.01f,
				map->light[i].xyz[2]
			);

		}

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}

}

void DrawPointMouse()
{
	POINT mouse;
	GetCursorPos(&mouse);

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POINT);
		
		glVertex2d(
			mouse.x,
			mouse.y
		);
	glEnd();

}

// END 