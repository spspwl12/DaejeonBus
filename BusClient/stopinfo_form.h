#include <Windows.h>
#include <stdio.h>
#include "resource.h"

INT_PTR CALLBACK StopInfoFormProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char tmp[50];
	static BUSSTOP *busstop;
	static DWORD pos;
	static HWND hList;
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

			hList = GetDlgItem(hDlg, IDC_LC_BUSSTOPINFO_THROUGHBUS);
			ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			_ADDCOL(hList, 0, 50, "노선");
			_ADDCOL(hList, 1, 80, "시간");
			_ADDCOL(hList, 2, 120, "방면");
			_ADDCOL(hList, 3, -1, "");

			BUS_TO_LIST(hList, busstop->busid);
			break;
		case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			int j;
			BOOL Up;

			if ((j = ((LPNMLISTVIEW)lParam)->iItem) >= 0)
			{
				if (lpnmhdr->hwndFrom == hList)
				{
					if (lpnmhdr->code == NM_DBLCLK)
					{
						_LISTGETITEM(hList, j, 3, tmp, sizeof(tmp));
						Up = *tmp == 'U';
						i = atoi(tmp+1);

						EndDialog(hDlg, (LPARAM)FindBusID(i, Up));
					}
				}
			}

			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDC_BT_BUSSTOPINFO_CLOSE:
					EndDialog(hDlg, 0);
					break;
			}
			break;
		}
	}

	return (INT_PTR)FALSE;
}