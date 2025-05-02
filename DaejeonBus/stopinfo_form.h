#include <Windows.h>
#include <stdio.h>
#include "resource.h"

INT_PTR CALLBACK StopInfoFormProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char tmp[50];
	static BUSSTOP *busstop;
	static DWORD pos;
	RECT rt, wrt;
	LONG cx, cy;
	int i;

	switch (iMsg)
	{
		case WM_USER + 0xCC:
			busstop = (BUSSTOP*)lParam;
			pos = (DWORD)wParam;
			break;
		case WM_INITDIALOG:
			GetWindowRect(hDlg, &rt);
			GetWindowRect(GetDesktopWindow(), &wrt);

			cx = rt.left + LOWORD(pos);
			cy = rt.top + HIWORD(pos);

			if (cx + (rt.right - rt.left) >= wrt.right) cx = wrt.right - (rt.right - rt.left);
			if (cy + (rt.bottom - rt.top) >= wrt.bottom) cy = wrt.bottom - (rt.bottom - rt.top);

			SetWindowPos(hDlg, NULL, cx, cy, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

			sprintf(tmp, "정류장 이름 : %s", busstop->STOPNAME);
			SendDlgItemMessage(hDlg, IDC_ST_BUSSTOPINFO_STOPNAME, WM_SETTEXT, 0, (LPARAM)tmp);

			sprintf(tmp, "정류장 번호 : %d", busstop->stopno);
			SendDlgItemMessage(hDlg, IDC_ST_BUSSTOPINFO_STOPID, WM_SETTEXT, 0, (LPARAM)tmp);

			sprintf(tmp, "버스 번호 : %d", busstop->busid);
			SendDlgItemMessage(hDlg, IDC_ST_BUSSTOPINFO_STOPBID, WM_SETTEXT, 0, (LPARAM)tmp);

			BUS_TO_LIST(GetDlgItem(hDlg, IDC_LT_BUSSTOPINFO_THROUGHBUS), busstop->busid);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_LT_BUSSTOPINFO_THROUGHBUS:
					if (HIWORD(wParam) == LBN_DBLCLK)
					{
						if ((i = SendDlgItemMessage(hDlg, IDC_LT_BUSSTOPINFO_THROUGHBUS, LB_GETCURSEL, 0, 0)) >= 0)
							EndDialog(hDlg, (LPARAM)LIST_TO_BUS(busstop->busid, i));
					}
					break;
				case IDOK:
				case IDC_BT_BUSSTOPINFO_CLOSE:
					EndDialog(hDlg, 0);
					break;
			}
			break;
		case WM_NOTIFY:
			break;
	}

	return (INT_PTR)FALSE;
}