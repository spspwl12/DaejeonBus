#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>
#include "resource.h"
#include "daejeonbus_form.h"
#include "hdr.h"

INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInst = hInstance;

	srand((unsigned int)time(NULL));
	InitGDI();
	RegisterMapClass(hInstance);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DL_DAEJEONBUS), NULL, DaejeonBusFormProc);

	return 0;
}