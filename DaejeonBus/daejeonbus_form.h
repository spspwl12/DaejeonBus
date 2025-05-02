#ifndef DAEHEONBUS_FORM_H
#define DAEHEONBUS_FORM_H

#define BUSSTOP_CT_MIN 2000
#define BUSSTOP_CT_MAX 2023

#define BUS_CT_MIN 3000
#define BUS_CT_MAX 3024

#include "listview.h"
#include "dbfunc.h"
#include "MapEngine.h"
#include "stopinfo_form.h"
#include "addbus_form.h"
#include "addbusstop_form.h"
#include "hdr.h"
#include "util.h"
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")

VOID CALLBACK ClickEvent(HWND, BYTE, INT, DWORD, LPVOID);
VOID DrawRoute(BUS*, double*, double*);
VOID DrawStop(BUS*, double*, double*);
BOOL SearchLL = FALSE;
BOOL PickMode = FALSE;
BUS *PickBus;
HWND hStateBar;
HWND MyDlg;

INT_PTR CALLBACK DaejeonBusFormProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hListView[4], hMap = NULL;
	static int Get_ListView_Index[4] = { -1 };
	char tmp[50];
	int i, j, k, l, m;
	LPNMHDR lpnmhdr;
	BUS *recvBus; 
	BUSSTOP *recvBusStop;
	double _lt, _lg, _flt, _flg;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			MyDlg = hDlg;

			// 모든 컨트롤 숨기기
			ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
			ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);

			hListView[0] = GetDlgItem(hDlg, IDC_LC_BUSSTOP_VIEW);
			ListView_SetExtendedListViewStyle(hListView[0], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			hListView[1] = GetDlgItem(hDlg, IDC_LC_BUSSTOP_PASSBUS);
			ListView_SetExtendedListViewStyle(hListView[1], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			hListView[2] = GetDlgItem(hDlg, IDC_LC_BUS_LISTBUS);
			ListView_SetExtendedListViewStyle(hListView[2], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			hListView[3] = GetDlgItem(hDlg, IDC_LC_BUS_PASSBUSSTOP);
			ListView_SetExtendedListViewStyle(hListView[3], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			_ADDCOL(hListView[0], 0, 80,  "번호");
			_ADDCOL(hListView[0], 1, 600, "이름");
			_ADDCOL(hListView[0], 2, -1, "");
			_ADDCOL(hListView[0], 3, -1, "");
			_ADDCOL(hListView[0], 4, -1, "");

			_ADDCOL(hListView[1], 0, 120, "방면");
			_ADDCOL(hListView[1], 1, 80, "종류");
			_ADDCOL(hListView[1], 2, 80, "노선");
			_ADDCOL(hListView[1], 3, -1, "");

			_ADDCOL(hListView[2], 0, 80, "번호");
			_ADDCOL(hListView[2], 1, 180, "방면");
			_ADDCOL(hListView[2], 2, 80, "종류");
			_ADDCOL(hListView[2], 3, 100, "노선");
			_ADDCOL(hListView[2], 4, -1, "");

			_ADDCOL(hListView[3], 0, 40, "순번");
			_ADDCOL(hListView[3], 1, 80, "번호");
			_ADDCOL(hListView[3], 2, 200, "이름");
			_ADDCOL(hListView[3], 3, -1, "");

			SendDlgItemMessage(hDlg, IDC_CT_BUSSTOP_SEARCHTYPE, CB_ADDSTRING, 0, (LPARAM)"이름");
			SendDlgItemMessage(hDlg, IDC_CT_BUSSTOP_SEARCHTYPE, CB_ADDSTRING, 0, (LPARAM)"번호");

			SendDlgItemMessage(hDlg, IDC_CT_BUS_SEARCHTYPE, CB_ADDSTRING, 0, (LPARAM)"노선");
			SendDlgItemMessage(hDlg, IDC_CT_BUS_SEARCHTYPE, CB_ADDSTRING, 0, (LPARAM)"방면");
			SendDlgItemMessage(hDlg, IDC_CT_BUS_SEARCHTYPE, CB_ADDSTRING, 0, (LPARAM)"종류");

			ComboBox_AddString(GetDlgItem(hDlg, IDC_CT_BUS_TYPE), 9, (LPSTR*)TypeName);

			LOAD_BUSSTOP_DB();
			LOAD_BUS_DB();
			LOAD_ROUTE_DB();
			LOAD_BUSROUTE_DB();

			InitCommonControls();
			hStateBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, NULL, hDlg, 0 );

			break;
		case WM_SIZE:
			SendMessage(hStateBar, WM_SIZE, wParam, lParam);
			break;
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				switch (LOWORD(wParam))
				{
					case ID_MB_DBMSAVE:
					{
						SAVE_BUSSTOP_DB();
						SAVE_BUS_DB();
						SAVE_BUSROUTE();

						break;
					}
					case IDC_RC_DBM_SELBUS:
					{
						if (IsDlgButtonChecked(hDlg, wParam))
						{
							i = DB_TO_LIST_TO_BUS(hListView[2]);
							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
							ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_SHOW);

							sprintf(tmp, "%d 개", i);
							SendMessage(hStateBar, SB_SETTEXT, 0, (LPARAM)tmp);
						}
						break;
					}
					case IDC_RC_DBM_SELSTOP:
					{
						if (IsDlgButtonChecked(hDlg, wParam))
						{
							ShowWindow(hListView[0], SW_HIDE);
							i = DB_TO_LIST_TO_BUSSTOP(hListView[0]);
							ShowWindow(hListView[0], SW_SHOW);
							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_SHOW);
							ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);

							sprintf(tmp, "%d 개", i);
							SendMessage(hStateBar, SB_SETTEXT, 0, (LPARAM)tmp);
						}
						break;
					}
					case IDC_BT_BUSSTOP_SEARCH:
						GetDlgItemText(hDlg, IDC_EC_BUSSTOP_SEARCH, tmp, 50);
						i = SendDlgItemMessage(hDlg, IDC_CT_BUSSTOP_SEARCHTYPE, CB_GETCURSEL, 0, 0);

						if (i >= 0 && strlen(tmp) > 0)
						{
							SEARCH_TO_LIST_FOR_BUSSTOP(hListView[0], hStateBar, i + 1, tmp);
							SearchLL = TRUE;
						}
						else
							MessageBox(hDlg, "검색 타입과 글자를 입력하세요.", "알림", MB_ICONEXCLAMATION);
						break;
					case ID_MB_END:
						PostQuitMessage(0);
						break;
					case IDC_BT_BUSSTOP_ADD:
					{
						int ctrlID[] = { 2004 , 2005, 2006, 2007, 2008 };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "ddffs50", &j, &k, &_lt, &_lg, tmp))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if (!ADD_BUSSTOP(k, j, _lt, _lg, tmp))
						{
							MessageBox(hDlg, "버스 정류장 추가 실패!\n중복된 정류장 ID 이거나 알 수 없는 오류로 인해 실패했습니다.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						_ADDLISTBATCH(hListView[0], BUSSTOP_COUNT - 1, 5, 0, "dsffd", &k, tmp, &_lt, &_lg, &j);

						sprintf(tmp, "%d 개", BUSSTOP_COUNT);
						SendMessage(hStateBar, SB_SETTEXT, 0, (LPARAM)tmp);
						break;
					}
					case IDC_BT_BUSSTOP_DEL:
					{
						int ctrlID[] = { 2004 };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "d", &j))
						{
							MessageBox(hDlg, "올바른 ID를 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if (!DEL_BUSSTOP(j))
						{
							MessageBox(hDlg, "없는 정류장이거나 올바르지 않은 ID값 입니다.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						_DELLISTFORID(hListView[0], 4, j);
						--BUSSTOP_COUNT;

						sprintf(tmp, "%d 개", BUSSTOP_COUNT);
						SendMessage(hStateBar, SB_SETTEXT, 0, (LPARAM)tmp);
						SendMessage(hListView[1], LVM_DELETEALLITEMS, 0, 0);

						break;
					}
					case IDC_BT_BUSSTOP_MODIFY:
					{
						int ctrlID[] = { 2004 , 2005, 2006, 2007, 2008 };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "ddffs50", &j, &k, &_lt, &_lg, tmp))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if (!MODIFY_BUSSTOP(k, j, _lt, _lg, tmp))
						{
							MessageBox(hDlg, "버스 정류장 수정 실패!\n존재하지 않는 ID 입니다.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if ((l = _SEARCHLIST(hListView[0], 4, tmp)) >= 0)
							_ADDLISTBATCH(hListView[0], l, 5, 1, "dsffd", &k, tmp, &_lt, &_lg, &j);

						break;
					}
					case IDC_BT_BUSSTOP_PICKMAP:
					{
						SetClickEventFunc(ClickEvent);

						int ctrlID[] = { IDC_EC_BUSSTOP_LATI , IDC_EC_BUSSTOP_LONG };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "ff", &_lt, &_lg))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						PickMode = TRUE;
						ClearMarkUp();
						SetMarkUp(2, _lt, _lg, 0, 0, 4, 0xFF, NULL);
						hMap = ShowMapForm(MODE_PICK, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 14, _lt, _lg);
						break;
					}
					case IDC_BT_BUSSTOP_ADDBUS:
					{
						BUS *sel = (BUS*)DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DL_ADDBUS), hDlg, AddBusFormProc);
						
						if (sel)
						{
							int ctrlID[] = { IDC_EC_BUSSTOP_ID };

							if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "d", &j))
							{
								MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
								return (INT_PTR)FALSE;
							}

							if (!ADD_BUSSTOP_IN_BUS(sel, j))
							{
								MessageBox(hDlg, "버스 추가 실패", "오류", MB_ICONEXCLAMATION);
								return (INT_PTR)FALSE;
							}

							k = SendMessage(hListView[1], LVM_GETITEMCOUNT, 0, 0);

							sprintf(tmp, "%c%d", sel->Up ? 'U' : 'D', sel->id);
							_ADDLISTBATCH(hListView[1], k, 4, 0, "ssds", sel->BUSENDNAME, CovBusType(sel->Type), &sel->nID, tmp);
						}

						break;
					}
					case IDC_BT_BUSSTOP_DELBUS:
					{
						if ((i = Get_ListView_Index[1]) >= 0)
						{
							if ((l = SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_ID, WM_GETTEXTLENGTH, 0, 0)) >= 0)
							{
								_LISTGETITEM(hListView[1], i, 3, tmp, sizeof(tmp));
								j = atoi(&tmp[1]);

								m = (BYTE)*tmp;
								memset(tmp, 0, sizeof(tmp));
								SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_ID, WM_GETTEXT, (WPARAM)(l + 1), (LPARAM)tmp);
								k = atoi(tmp);

								if (j > 0 && DEL_BUSSTOP_IN_BUS(k, j, m == 'U' ? 1 : 0))
								{
									sprintf(tmp, "%c%d", m, j);
									_DELLISTFORSTR(hListView[1], 3, tmp);
									Get_ListView_Index[1] = -1;
								}
							}
						}
						break;
					}
					case IDC_BT_BUS_SEARCH:
					{
						// 버스 검색
						GetDlgItemText(hDlg, IDC_EC_BUS_SEARCHTEXT, tmp, 50);
						i = SendDlgItemMessage(hDlg, IDC_CT_BUS_SEARCHTYPE, CB_GETCURSEL, 0, 0);

						if (i >= 0 && strlen(tmp) > 0)
						{
							SEARCH_TO_LIST_FOR_BUS(hListView[2], hStateBar, i + 1, tmp);
							SearchLL = TRUE;
						}
						else
							MessageBox(hDlg, "검색 타입과 글자를 입력하세요.", "알림", MB_ICONEXCLAMATION);

						break;
					}
					case IDC_BT_BUS_ADD:
					{
						// 버스를 추가하는 함수
						int ctrlID[] = { IDC_EC_BUS_ID , IDC_EC_BUS_DIRECT, IDC_EC_BUS_UPDOWN, IDC_CT_BUS_TYPE, IDC_EC_BUS_ROUTE };

						if( !GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "ds50dcd", &i, tmp, &j, &k, &l)  )
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						++k;

						if (!ADD_BUS(i, j, l, k, tmp))
						{
							MessageBox(hDlg, "버스 추가 실패!\n중복된 버스 ID 이거나 알 수 없는 오류로 인해 실패했습니다.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						_ADDLISTBATCH(hListView[2], BUS_COUNT - 1, 5, 0, "dssdd", &i, tmp, CovBusType(k), &l, &j);

						sprintf(tmp, "%d 개", BUS_COUNT);
						SendMessage(hStateBar, SB_SETTEXT, 0, (LPARAM)tmp);

						break;

					}
					case IDC_BT_BUS_DEL:
					{
						// 버스 삭제하는 함수

						int ctrlID[] = { IDC_EC_BUS_ID , IDC_EC_BUS_UPDOWN };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "dd", &i, &j))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if (!DEL_BUS(i, j))
						{
							MessageBox(hDlg, "없는 정류장이거나 올바르지 않은 ID값 입니다.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						_DELLISTFORIDUP(hListView[2], i, j);
						--BUS_COUNT;

						sprintf(tmp, "%d 개", BUS_COUNT);
						SendMessage(hStateBar, SB_SETTEXT, 0, (LPARAM)tmp);
						SendMessage(hListView[3], LVM_DELETEALLITEMS, 0, 0);

						break;
					}
					case IDC_BT_BUS_MODIFY:
					{
						// 버스 수정하는 함수

						int ctrlID[] = { IDC_EC_BUS_ID , IDC_EC_BUS_DIRECT, IDC_EC_BUS_UPDOWN, IDC_CT_BUS_TYPE, IDC_EC_BUS_ROUTE };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "ds50dcd", &i, tmp, &j, &k, &l))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}
						++k;

						if (!MODIFY_BUS(i, j, l, k, tmp))
						{
							MessageBox(hDlg, "없는 정류장이거나 올바르지 않은 ID값 입니다.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if ((m = _SEARCHLISTFORIDUP(hListView[2], i, j)) >= 0)
							_ADDLISTBATCH(hListView[2], m, 5, 1, "dssdd", &i, tmp, CovBusType(k), &l, &j);

						break;
					}
					case IDC_BT_BUS_ADDSTOP:
					{
						if( !(recvBusStop = (BUSSTOP*)DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DL_ADDBUSSTOP), hDlg, AddBusStopFormProc) ))
							return (INT_PTR)FALSE;

						int ctrlID[] = { IDC_EC_BUS_ID , IDC_EC_BUS_UPDOWN };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "dd", &i, &j))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if (recvBus = FindBusID(i, (BOOL)j))
						{
							if (ADD_BUSSTOP_IN_BUS(recvBus, recvBusStop->busid))
							{
								m = SendMessage(hListView[3], LVM_GETITEMCOUNT, 0, 0);
								i = m + 1;
								_ADDLISTBATCH(hListView[3], m, 4, 0, "ddsd", &i, &recvBusStop->stopno, recvBusStop->STOPNAME, &recvBusStop->busid);
							}
						}

						break;
					}
					case IDC_BT_BUS_DELSTOP:
					{
						if ((i = Get_ListView_Index[3]) < 0)
							return (INT_PTR)FALSE;

						_LISTGETITEM(hListView[3], i, 3, tmp, sizeof(tmp));

						if( (j = atoi(tmp)) == -1 )
							return (INT_PTR)FALSE;

						int ctrlID[] = { IDC_EC_BUS_ID , IDC_EC_BUS_UPDOWN };

						if (!GetDlgItemBatch(hDlg, ctrlID, sizeof(ctrlID) / sizeof(int), "dd", &k, &l))
						{
							MessageBox(hDlg, "올바른 값을 입력하세요.", "오류", MB_ICONEXCLAMATION);
							return (INT_PTR)FALSE;
						}

						if (DEL_BUSSTOP_IN_BUS(j, k, l))
						{
							_DELLISTFORID(hListView[3], 3, j);
							Get_ListView_Index[3] = -1;
						}

						break;
					}
					case IDC_BT_BUS_MUPSTOP:
						if (Get_ListView_Index[3] > 0)
						{
							int ctrlID[2] = { IDC_EC_BUS_ID , IDC_EC_BUS_UPDOWN };
							GetDlgItemBatch(hDlg, ctrlID, 2, "dd", &j, &k);

							if (recvBus = FindBusID(j, k))
							{
								SwapBusStop(recvBus, Get_ListView_Index[3], Get_ListView_Index[3] - 1);
								_SWAPLIST(hListView[3], Get_ListView_Index[3], Get_ListView_Index[3] - 1, 1, 3, 100);
								Get_ListView_Index[3]--;
								_SELLIST(hListView[3], Get_ListView_Index[3]);
							}
						}
						break;
					case IDC_BT_BUS_MDOWNSTOP:
						if (Get_ListView_Index[3] < SendMessage(hListView[3], LVM_GETITEMCOUNT, 0, 0))
						{
							int ctrlID[2] = { IDC_EC_BUS_ID , IDC_EC_BUS_UPDOWN };
							GetDlgItemBatch(hDlg, ctrlID, 2, "dd", &j, &k);

							if (recvBus = FindBusID(j, k))
							{
								SwapBusStop(recvBus, Get_ListView_Index[3], Get_ListView_Index[3] + 1);
								_SWAPLIST(hListView[3], Get_ListView_Index[3], Get_ListView_Index[3] + 1, 1, 3, 100);
								Get_ListView_Index[3]++;
								_SELLIST(hListView[3], Get_ListView_Index[3]);
							}
						}
						break;
					case IDC_BT_BUS_PATH:
						if (Get_ListView_Index[3] >= 0)
						{
							int ctrlID[2] = { IDC_EC_BUS_ID , IDC_EC_BUS_UPDOWN };
							GetDlgItemBatch(hDlg, ctrlID, 2, "dd", &j, &k);

							if (recvBus = FindBusID(j, k))
							{
								PickBus = recvBus;
								ClearMarkUp();
								_lt = _lg = 0.0;
								DrawRoute(recvBus, &_lt, &_lg);
								SetMapRoute((PMapROUTE)_ROTST);
								SetClickEventFunc(ClickEvent);

								if (_lt == 0 || _lg == 0)
								{
									_lt = 36.351417;
									_lg = 127.386721;
								}

								PickMode = FALSE;
								hMap = ShowMapForm(MODE_ROUTE, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 12, _lt, _lg);
								InvalidateRgn(hMap, NULL, FALSE);
							}
						}
						break;
				}
			}
			break;
		case WM_NOTIFY:
			lpnmhdr = (LPNMHDR)lParam;
			l = ((LPNMLISTVIEW)lParam)->iItem;

			// 정류장
			if (l >= 0)
			{
				if (lpnmhdr->hwndFrom == hListView[0])
				{
					if (lpnmhdr->code == LVN_ITEMCHANGED)
					{
						Get_ListView_Index[0] = l;

						_LISTGETITEM(hListView[0], l, 0, tmp, sizeof(tmp));
						if (*tmp == 'U') memset(tmp, 0, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_NUM, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[0], l, 1, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_NAME, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[0], l, 2, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_LATI, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[0], l, 3, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_LONG, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[0], l, 4, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUSSTOP_ID, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						// 딜레이 최소화
						ShowWindow(hListView[1], SW_HIDE);
						BUS_TO_LISTVIEW(hListView[1], atoi(tmp));
						ShowWindow(hListView[1], SW_SHOW);
					}
					else if (lpnmhdr->code == NM_DBLCLK)
					{
						_LISTGETITEM(hListView[0], l, 2, tmp, sizeof(tmp));
						_flt = strtod(tmp, NULL);
						_LISTGETITEM(hListView[0], l, 3, tmp, sizeof(tmp));
						_flg = strtod(tmp, NULL);
						_LISTGETITEM(hListView[0], l, 4, tmp, sizeof(tmp));
						k = atoi(tmp);

						ClearMarkUp();

						SetMarkUp(2, _flt, _flg, 0, 0, 4, 0x00FF00, FindBusStopUID(k));

						if (SearchLL)
						{
							for (j = 0; j < SendMessage(hListView[0], LVM_GETITEMCOUNT, 0, 0); ++j)
							{
								if (j != l)
								{
									_LISTGETITEM(hListView[0], j, 2, tmp, sizeof(tmp));
									_lt = strtod(tmp, NULL);
									_LISTGETITEM(hListView[0], j, 3, tmp, sizeof(tmp));
									_lg = strtod(tmp, NULL);
									_LISTGETITEM(hListView[0], j, 4, tmp, sizeof(tmp));
									i = atoi(tmp);

									SetMarkUp(2, _lt, _lg, 0, 0, 4, 0x0000FF, FindBusStopUID(i));

									if (j > DRAW_MARK_MAX_COUNT)
										break;
								}
							}
						}

						PickMode = FALSE;
						SetClickEventFunc(ClickEvent);
						hMap = ShowMapForm(MODE_NORMAL, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 14, _flt, _flg);
					}
				}
				else if (lpnmhdr->hwndFrom == hListView[1])
				{
					if (lpnmhdr->code == LVN_ITEMCHANGED)
						Get_ListView_Index[1] = l;
				}
				else if (lpnmhdr->hwndFrom == hListView[2]) //버스
				{
					if (lpnmhdr->code == LVN_ITEMCHANGED)
					{
						Get_ListView_Index[2] = l;

						_LISTGETITEM(hListView[2], l, 0, tmp, sizeof(tmp));
						j = atoi(tmp);
						SendDlgItemMessage(hDlg, IDC_EC_BUS_ID, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[2], l, 1, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUS_DIRECT, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[2], l, 2, tmp, sizeof(tmp));

						for (i = 0; i < 9; ++i)
						{
							if (!strncmp(TypeName[i], tmp, strlen(tmp)))
							{
								SendDlgItemMessage(hDlg, IDC_CT_BUS_TYPE, CB_SETCURSEL, (WPARAM)i, 0);
								break;
							}
						}

						_LISTGETITEM(hListView[2], l, 3, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUS_ROUTE, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

						_LISTGETITEM(hListView[2], l, 4, tmp, sizeof(tmp));
						SendDlgItemMessage(hDlg, IDC_EC_BUS_UPDOWN, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);


						// 딜레이 최소화
						ShowWindow(hListView[3], SW_HIDE);
						BUSSTOP_TO_LISTVIEW(hListView[3], j, atoi(tmp));
						ShowWindow(hListView[3], SW_SHOW);
					}
					else if (lpnmhdr->code == NM_DBLCLK)
					{
						_LISTGETITEM(hListView[2], l, 0, tmp, sizeof(tmp));
						i = atoi(tmp);
						_LISTGETITEM(hListView[2], l, 4, tmp, sizeof(tmp));
						j = atoi(tmp);

						if (i)
						{
							BUS *Find_Bus = FindBusID(i, j);

							if (Find_Bus)
							{
								SetClickEventFunc(ClickEvent);
								SendMessage(hDlg, WM_USER + 0xCF, (WPARAM)NULL, (LPARAM)Find_Bus);
							}
						}
					}
				}
				else if (lpnmhdr->hwndFrom == hListView[3])
				{
					if (lpnmhdr->code == LVN_ITEMCHANGED)
					{
						Get_ListView_Index[3] = l;
						EnableWindow(GetDlgItem(hDlg, IDC_BT_BUS_MUPSTOP), l != 0);
						EnableWindow(GetDlgItem(hDlg, IDC_BT_BUS_MDOWNSTOP), l != (SendMessage(hListView[3], LVM_GETITEMCOUNT, 0, 0) - 1));
					}
				}
			}

			break;
		case WM_USER + 0xCF:
			if (recvBus = (BUS*)lParam)
			{
				if (!recvBus->cntBstop)
				{
					MessageBox(hDlg, "해당 버스는 경유하는 정류장이 없습니다.", "알림", MB_ICONINFORMATION);
					return (INT_PTR)FALSE;
				}

				ClearMarkUp();

				DrawRoute(recvBus, NULL, NULL);
				DrawStop(recvBus, &_flt, &_flg);

				PickMode = FALSE;
				hMap = ShowMapForm(MODE_NORMAL, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 12, _flt, _flg);
				InvalidateRgn(hMap, NULL, FALSE);
			}
			break;
		case WM_CLOSE:
			ReleaseDB();
			ReleaseSeek();
			PostQuitMessage(0);
			break;
	}

	return (INT_PTR)FALSE;
}

VOID DrawRoute(BUS *recvBus, double *center1, double *center2)
{
	int i, j, k = -1, l;
	double _lt, _lg;
	ROUTE *_recvBusRt;

	if (!recvBus)
		return;

	for (j = 0; j < (int)recvBus->cntRoute; ++j)
	{
		if (k != recvBus->p_BusRoute[j])
		{
			k = recvBus->p_BusRoute[j];

			if ((_recvBusRt = FindRouteID(k)) && (l = (int)(_recvBusRt->rcnt / 2)))
			{
				for (i = 0; i < l; ++i)
				{
					_lt = _recvBusRt->_Lat[i];
					_lg = _recvBusRt->_Lng[i];

					if (center1 && center2)
					{
						if (l == 1)
						{
							*center1 = _lt;
							*center2 = _lg;
						}
						else if (i == l / 2 - 1)
						{
							*center1 = _lt;
							*center2 = _lg;
						}
					}

					if (i + 1 < l)
						SetMarkUp(3, _lt, _lg, _recvBusRt->_Lat[i + 1], _recvBusRt->_Lng[i + 1], 5, 0x0000FF, NULL);

				}
			}
		}
	}
}

VOID DrawStop(BUS *recvBus, double *center1, double *center2)
{
	UINT i;
	BUSSTOP *recvBusStop;
	double _lt, _lg;

	if (!recvBus)
		return;

	for (i = 0; i < recvBus->cntBstop; ++i)
	{
		if (recvBusStop = recvBus->p_BusStop[i])
		{
			_lt = recvBusStop->latitude;
			_lg = recvBusStop->longitude;

			if (recvBus->cntBstop == 1)
			{
				*center1 = _lt;
				*center2 = _lg;
			}
			else if (i == recvBus->cntBstop / 2 - 1)
			{
				*center1 = _lt;
				*center2 = _lg;
			}

			SetMarkUp(2, _lt, _lg, 0, 0, 4, 0x00FF00, recvBusStop);
		}
	}
}


VOID CALLBACK ClickEvent(HWND hWnd, BYTE Mode, INT nIndex, DWORD lParam, LPVOID lparg)
{
	LPARAM p_result;
	char tmp[50];

	switch (Mode)
	{
		case MODE_NORMAL:
		{
			StopInfoFormProc(NULL, WM_USER + 0xCC, (WPARAM)lParam, (LPARAM)lparg);

			if (p_result = (LPARAM)DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DL_BUSSTOPINFO), hWnd, StopInfoFormProc))
				DaejeonBusFormProc(NULL, WM_USER + 0xCF, NULL, p_result);

			break;
		}
		case MODE_PICK:
		{
			if (PickMode)
			{
				sprintf(tmp, "%lf", Picked_Lat());
				SendDlgItemMessage(MyDlg, IDC_EC_BUSSTOP_LATI, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);

				sprintf(tmp, "%lf", Picked_Long());
				SendDlgItemMessage(MyDlg, IDC_EC_BUSSTOP_LONG, WM_SETTEXT, (WPARAM)strlen(tmp), (LPARAM)tmp);
			}
			break;
		}
		case MODE_ROUTE:
		{
			if (PickBus)
			{
				UINT nRoute = (UINT)lParam;
				ROUTE *next = _ROTST;

				if (DEL_BUS_ROUTE(PickBus, nRoute))
				{
					// 지우개

					while (next)
					{
						if (next->rid == nRoute)
						{
							for (int j = 0; j < (int)(next->rcnt / 2); ++j)
								DeleteMarkUp(3, next->_Lat[j], next->_Lng[j], next->_Lat[j + 1], next->_Lng[j + 1]);

							break;
						}

						next = next->next;
					}
				}
				else
				{
					// 연필

					if (ADD_BUS_ROUTE(PickBus, nRoute))
					{
						while (next)
						{
							if (next->rid == nRoute)
							{
								for (int j = 0; j < (int)(next->rcnt / 2); ++j)
									SetMarkUp(3, next->_Lat[j], next->_Lng[j], next->_Lat[j + 1], next->_Lng[j + 1], 5, 0xFF, NULL);

								break;
							}

							next = next->next;
						}
					}
				}
			}

			break;
		}
	}
}
#endif