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

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DL_SELECTOR), NULL, DlgProc);

	return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:

			break;
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				switch (LOWORD(wParam))
				{
					case IDC_BT_SELECTOR_DAEJEONBUS:
						EndDialog(hDlg, 0);
						DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DL_DAEJEONBUS), NULL, DaejeonBusFormProc);
						break;
					case IDC_BT_SELECTOR_TASHU:
						break;
				}
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
	}
	return (INT_PTR)FALSE;
}