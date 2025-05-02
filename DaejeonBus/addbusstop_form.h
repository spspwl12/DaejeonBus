#ifndef ADDBUSSTOP_FORM_H
#define ADDBUSSTOP_FORM_H
#include "dbfunc.h"

INT_PTR CALLBACK AddBusStopFormProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hListView = NULL;
	char tmp[50];

	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			SendDlgItemMessage(hDlg, IDC_CT_ADDBUSSTOP_TYPE, CB_ADDSTRING, 0, (LPARAM)"이름");
			SendDlgItemMessage(hDlg, IDC_CT_ADDBUSSTOP_TYPE, CB_ADDSTRING, 0, (LPARAM)"번호");

			hListView = GetDlgItem(hDlg, IDC_LC_ADDBUSSTOP_LIST);
			SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)0, (LPARAM)(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES));

			ShowWindow(hListView, SW_HIDE);

			_ADDCOL(hListView, 0, 80, "번호");
			_ADDCOL(hListView, 1, 400, "이름");
			_ADDCOL(hListView, 2, -1, "");

			DB_TO_LIST_TO_BUSSTOP_SIMPLE(hListView);

			ShowWindow(hListView, SW_SHOW);

			break;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_BT_ADDBUSSTOP_SEARCH)
			{
				char tmp[50];
				int i;

				GetDlgItemText(hDlg, IDC_EC_ADDBUSSTOP_SEARCH, tmp, sizeof(tmp));
				i = SendDlgItemMessage(hDlg, IDC_CT_ADDBUSSTOP_TYPE, CB_GETCURSEL, 0, 0);

				if (i >= 0 && strlen(tmp) > 0)
				{
					ShowWindow(hListView, SW_HIDE);
					SEARCH_TO_LIST_FOR_BUSSTOP_SIMPLE(hListView, i + 1, tmp);
					ShowWindow(hListView, SW_SHOW);
				}
				else
					MessageBox(hDlg, "검색 타입과 글자를 입력하세요.", "알림", MB_ICONEXCLAMATION);
			}

			break;
		}
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->hwndFrom == hListView && ((LPNMLISTVIEW)lParam)->iItem >= 0)
			{
				if (((LPNMHDR)lParam)->code == NM_DBLCLK)
				{
					_LISTGETITEM(hListView, ((LPNMLISTVIEW)lParam)->iItem, 2, tmp, sizeof(tmp));
					EndDialog(hDlg, (INT_PTR)FindBusStopUID(atoi(tmp)));
				}
			}

			break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, (INT_PTR)NULL);
			break;
	}

	return (INT_PTR)FALSE;
}
#endif