#ifndef ADDBUS_FORM_H
#define ADDBUS_FORM_H
#include "dbfunc.h"

BUS *SelBus = NULL;

INT_PTR CALLBACK AddBusFormProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	BUS *next;
	char tmp[50];
	int i, j, k;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			for (i = 0; i < 9; ++i)
			{
				sprintf(tmp, "%s(%d)", TypeName[i], i + 1);
				SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_TYPE, CB_ADDSTRING, (WPARAM)i, (LPARAM)tmp);
			}
			break;
		case WM_COMMAND:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				switch (LOWORD(wParam))
				{
					case IDC_CT_ADDBUS_TYPE:
					{
						if ((i = SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_TYPE, CB_GETCURSEL, 0, 0)) >= 0)
						{
							SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_BUS, CB_RESETCONTENT, 0, 0);

							for (next = _BUSST, j = 0; next; next = next->next)
							{
								if (next->Type == (i + 1))
								{
									sprintf(tmp, "%d(%s)", next->nID, next->BUSENDNAME);
									SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_BUS, CB_ADDSTRING, (WPARAM)j, (LPARAM)tmp);

									++j;
								}
							}
						}
						else
						{
							SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_BUS, CB_RESETCONTENT, 0, 0);
							SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_BUS, CB_ADDSTRING, 0, (LPARAM)"먼저 종류를 선택하세요");
						}
						break;
					}
					case IDC_CT_ADDBUS_BUS:
					{
						if ((i = SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_TYPE, CB_GETCURSEL, 0, 0)) >= 0)
						{
							if ((k = SendDlgItemMessage(hDlg, IDC_CT_ADDBUS_BUS, CB_GETCURSEL, 0, 0)) >= 0)
							{
								for (next = _BUSST, j = 0; next; next = next->next)
								{
									if (next->Type == (i + 1))
									{
										if (k == j)
										{
											sprintf(tmp, "%d(%s)", next->nID, next->BUSENDNAME);
											SelBus = next;

											break;
										}

										++j;
									}
								}
							}
						}
						break;
					}
				}
			}
			else if (HIWORD(wParam) == BN_CLICKED)
			{
				if (LOWORD(wParam) == IDC_BT_ADDBUS_ADD)
				{
					EndDialog(hDlg, (INT_PTR)SelBus);
					return (INT_PTR)NULL;
				}
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
	}

	return (INT_PTR)FALSE;
}
#endif