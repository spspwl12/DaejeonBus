#ifndef MAP_ENGINE_H
#define MAP_ENGINE_H

#include <Windows.h>
#include <gdiplus.h>
#include <math.h>
#include <stdio.h>

#pragma comment(lib, "Gdiplus.lib")

#define MAPPATH 				"DATA\\MAP\\map"
#define HASHPATH 				"DATA\\MAP\\map.txt"

#define PI						3.1415926535897932384626433

#define IMG_W					13
#define IMG_H					11

#define FIRST_ZOOM				11
#define FIRST_ZOOM_SW			1747
#define FIRST_ZOOM_SH			800
#define FIRST_ZOOM_EW			1750
#define FIRST_ZOOM_EH			803

#define DRAW_MARK_MAX_COUNT		5000

#define MAP_INITDATA			(WM_USER + 0xE4)
#define MAP_WINDOWLIVE			(WM_USER + 0xE5)

#define MODE_NORMAL				0
#define MODE_PICK				1
#define MODE_ROUTE				2

typedef struct __MapParameter
{
	BYTE _Mode;
	UINT Zoom;
	double Lat;
	double Long;
}MapParameter, *PMapParameter;

typedef struct _MapHash
{
	DWORD Seek;
	DWORD Size;
}MapHash, *PMapHash;

typedef struct _MAPROUTE
{
	UINT rid;
	UINT rcnt;
	double *_Lat;
	double *_Lng;
	struct _MAPROUTE *next;
}MapROUTE, *PMapROUTE;

typedef VOID(CALLBACK* CLKEVENTPROC)(HWND, BYTE, INT, DWORD, LPVOID);

LRESULT CALLBACK	__MapViewerWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				IsViewMap();
VOID				RegisterMapClass(HINSTANCE);
HWND				ShowMapForm(BYTE, UINT, UINT, UINT, UINT, UINT, double, double);
Gdiplus::Image		*ext_png(LPCSTR, DWORD, DWORD);
VOID				SetZoom(BYTE);
VOID				SetLatLon(double, double, BYTE, LPPOINT);
VOID				SetLatLontoTileXY(double, double, BYTE, LPPOINT, LPPOINT);
VOID				TileXYToLatLon(LPPOINT, LPPOINT, BYTE, double*, double*);
BOOL				LoadSeek(LPCSTR);
BOOL				MakeBitmap(HWND, HBITMAP*, BYTE, INT, INT, BOOL);
VOID				ShowBitmap(HDC, HBITMAP*, long, long);
VOID				InitGDI();
VOID				InitMap(HWND, BYTE);
VOID				InitFont(UINT);
VOID				UpdateMapRECT(HWND);
VOID				DrawMap(HWND, HDC, BYTE);
VOID				SetMarkImage(HBITMAP, double, double, int, int);
VOID				SetMarkUp(BYTE, double, double, double, double, BYTE, COLORREF, LPVOID);
VOID				DrawSearchRoute(DWORD);
VOID				SetMapRoute(PMapROUTE);
VOID				CHANGEMAP(HWND, BYTE);
VOID				ReleaseData();
VOID				ReleaseSeek();
VOID				ReleaseFont();
VOID				ClearMarkUp();
VOID				SetClickEventFunc(CLKEVENTPROC);
INT					IsCursorInRect(DWORD);
#endif