#ifndef DAEHEONBUS_FORM_H
#define DAEHEONBUS_FORM_H

#define BUSSTOP_CT_MIN 2000
#define BUSSTOP_CT_MAX 2001

#define BUS_CT_MIN 3000
#define BUS_CT_MAX 3001

#define BUSTAB_CT_MIN 4000
#define BUSTAB_CT_MAX 4002

#define TIMER_MOVEBUS 101

#include "listview.h"
#include "dbfunc.h"
#include "MapEngine.h"
#include "stopinfo_form.h"
#include "hdr.h"
#include "util.h"
#include "virtualBus.h"
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "msimg32.lib")

struct  
{
	int type;
	int updown;
	int nID;
}_rClkEvent;

VOID CALLBACK ClickEvent(HWND, BYTE, INT, DWORD, LPVOID);

BOOL SearchLL = FALSE;
HBITMAP BusImage;
HFONT hBusFont;
HWND MyDlg, hBusTab;
INT PickBus = 0;
BOOL PickUpDown = 0;

INT_PTR CALLBACK DaejeonBusFormProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hListView[4], hMap = NULL, hTab;
	char tmp[50];
	int i, j, k, l;
	LPNMHDR lpnmhdr;
	BUS *recvBus; 
	static int BusStopMode = 0;
	BUSSTOP *recvBusStop;
	double _lt, _lg, _flt, _flg;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			MyDlg = hDlg;

			LOAD_BUSSTOP_DB();
			LOAD_BUS_DB();
			LOAD_ROUTE_DB();
			LOAD_BUSROUTE_DB();
			LOAD_FAVORITE_DB();

			InitCommonControls();

			hListView[0] = GetDlgItem(hDlg, IDC_LC_BUSSTOP_VIEW);
			ListView_SetExtendedListViewStyle(hListView[0], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			hListView[1] = GetDlgItem(hDlg, IDC_LC_BUS_LISTBUS);
			ListView_SetExtendedListViewStyle(hListView[1], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			hListView[2] = GetDlgItem(hDlg, IDC_LC_BUS_LISTSTOP);
			ListView_SetExtendedListViewStyle(hListView[2], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			hListView[3] = GetDlgItem(hDlg, IDC_LC_FAVORITE_LIST);
			ListView_SetExtendedListViewStyle(hListView[3], LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			RECT rt;
			GetClientRect(hListView[0], &rt);

			_ADDCOL(hListView[0], 0, 80, "");
			_ADDCOL(hListView[0], 1, rt.right - 100, "");
			_ADDCOL(hListView[0], 2, -1, "");

			_ADDCOL(hListView[1], 0, 80, "");
			_ADDCOL(hListView[1], 1, rt.right - 100, "");
			_ADDCOL(hListView[1], 2, -1, "");

			_ADDCOL(hListView[2], 0, 80, "위치");
			_ADDCOL(hListView[2], 1, rt.right - 100, "정류장 이름");
			_ADDCOL(hListView[2], 2, -1, "");

			_ADDCOL(hListView[3], 0, rt.right - 20, "");
			_ADDCOL(hListView[3], 1, -1, "");
			_ADDCOL(hListView[3], 2, -1, "");

			DB_TO_LIST_TO_BUS(hListView[1]);
	
			ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
			ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_SHOW);

			hTab = GetDlgItem(hDlg, IDC_TB_DBM_TAB);
			hBusTab = GetDlgItem(hDlg, IDC_TB_BUS_TAB);

			TabControl_AddItem(hTab, 0, "즐겨찾기");
			TabControl_AddItem(hTab, 1, "버스검색");
			TabControl_AddItem(hTab, 2, "정류장검색");

			TabControl_AddItem(hBusTab, 0, "");
			TabControl_AddItem(hBusTab, 1, "");

			TabCtrl_SetCurSel(hTab, 1);

			hBusFont = CreateFont(10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
				HANGUL_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_PITCH, 0, "굴림");

			BusImage = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BUSIMAGE));

			MakeBus();
			SetTimer(hDlg, TIMER_MOVEBUS, 5000, NULL);

			break;
		case WM_TIMER:
			if (LOWORD(wParam) == TIMER_MOVEBUS)
			{
				MoveBus();

				if(BusStopMode)
				{
					i = TabCtrl_GetCurSel(hBusTab);
					j = GetScrollPos(hListView[2], SB_VERT);
					BUS_TO_LIST_STOP(hListView[2], PickBus, i);
					ListView_Scroll(hListView[2], 0, j * 50);


					if (IsViewMap())
					{
						ClearMarkUp();

						recvBus = FindBusToUpDown(PickBus, PickUpDown);
						DrawRoute(recvBus, NULL, NULL);
						DrawBus(BusImage, recvBus);
						DrawStop(recvBus, &_flt, &_flg);

						InvalidateRgn(hMap, NULL, FALSE);
					}
					
				}
			}
			break;
		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					switch (LOWORD(wParam))
					{
						case IDC_EC_BUS_SEARCHTEXT:
						{
							GetDlgItemText(hDlg, IDC_EC_BUS_SEARCHTEXT, tmp, sizeof(tmp));
							SEARCH_TO_LIST_FOR_BUS(hListView[1], tmp);
							break;
						}
						case IDC_EC_BUSSTOP_SEARCH:
						{
							GetDlgItemText(hDlg, IDC_EC_BUSSTOP_SEARCH, tmp, sizeof(tmp));
							SearchLL = strlen(tmp) > 0;
							SEARCH_TO_LIST_FOR_BUSSTOP(hListView[0], tmp);
							break;
						}
					}
					break;
				}
				case BN_CLICKED:
				{
					switch (LOWORD(wParam))
					{
						case IDC_BT_BUS_PREV:
						{
							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
							ShowGroupsWindow(hDlg, BUSTAB_CT_MIN, BUSTAB_CT_MAX, SW_HIDE);
							ShowWindow(hBusTab, SW_HIDE);
							ShowWindow(hTab, SW_SHOW);

							switch (BusStopMode)
							{
								case 1:
									ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_SHOW);
									break;
								case 2:
									ShowGroupsWindow(hDlg, IDC_LC_FAVORITE_LIST, IDC_LC_FAVORITE_LIST, SW_SHOW);
									break;
							}

							BusStopMode = 0;
							break;
						}
						case IDC_BT_ADD_FAVORITE:
						{
							ADD_FAVORITE(_rClkEvent.type, _rClkEvent.updown, _rClkEvent.nID);
							SAVE_FAVORITE_DB();

							break;
						}
						case IDC_BT_ADD_FAVORITE_DOWN:
						{
							ADD_FAVORITE(0, 0, _rClkEvent.nID);
							SAVE_FAVORITE_DB();

							break;
						}
						case IDC_BT_ADD_FAVORITE_UP:
						{
							ADD_FAVORITE(0, 1, _rClkEvent.nID);
							SAVE_FAVORITE_DB();

							break;
						}
						case IDC_BT_DEL_FAVORITE:
						{
							DEL_FAVORITE(_rClkEvent.type, _rClkEvent.updown, _rClkEvent.nID);
							SAVE_FAVORITE_DB();

							DB_TO_LIST_TO_FAVORITE(hListView[3]);

							break;
						}
					}

					break;
				}
			}
	
			break;
		case WM_NOTIFY:
			lpnmhdr = (LPNMHDR)lParam;
			l = ((LPNMLISTVIEW)lParam)->iItem;

			if (lpnmhdr->code == TCN_SELCHANGE)
			{
				i = TabCtrl_GetCurSel(lpnmhdr->hwndFrom);

				if (lpnmhdr->hwndFrom == hTab)
				{
					switch (i)
					{
						case 0:
							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
							ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);
							ShowGroupsWindow(hDlg, IDC_LC_FAVORITE_LIST, IDC_LC_FAVORITE_LIST, SW_SHOW);
							DB_TO_LIST_TO_FAVORITE(hListView[3]);
							break;
						case 1:
							ShowGroupsWindow(hDlg, IDC_LC_FAVORITE_LIST, IDC_LC_FAVORITE_LIST, SW_HIDE);
							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
							ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_SHOW);
							DB_TO_LIST_TO_BUS(hListView[1]);
							break;
						case 2:
							ShowGroupsWindow(hDlg, IDC_LC_FAVORITE_LIST, IDC_LC_FAVORITE_LIST, SW_HIDE);
							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_SHOW);
							ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);
							break;
					}
				}
				else if (lpnmhdr->hwndFrom == hBusTab && PickBus)
				{
					BUS_TO_LIST_STOP(hListView[2], PickBus, i);

					ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
					ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);

					ShowGroupsWindow(hDlg, BUSTAB_CT_MIN, BUSTAB_CT_MAX, SW_SHOW);

					ShowWindow(hTab, SW_HIDE);
					ShowWindow(hBusTab, SW_SHOW);
				}
			}

			// 정류장
			if (l >= 0)
			{
				if (lpnmhdr->hwndFrom == hListView[0])
				{
					if (lpnmhdr->code == NM_DBLCLK)
					{
						_LISTGETITEM(hListView[0], l, 2, tmp, sizeof(tmp));
						k = atoi(tmp);

						if (recvBusStop = FindBusStopUID(k))
						{
							ClearMarkUp();

							_flt = recvBusStop->latitude;
							_flg = recvBusStop->longitude;

							SetMarkUp(2, _flt, _flg, 0, 0, 4, 0x00FF00, recvBusStop);

							if (SearchLL)
							{
								for (j = 0; j < SendMessage(hListView[0], LVM_GETITEMCOUNT, 0, 0); ++j)
								{
									if (j != l)
									{
										_LISTGETITEM(hListView[0], j, 2, tmp, sizeof(tmp));
										i = atoi(tmp);

										if (recvBusStop = FindBusStopUID(i))
										{
											_lt = recvBusStop->latitude;
											_lg = recvBusStop->longitude;

											SetMarkUp(2, _lt, _lg, 0, 0, 4, 0x0000FF, recvBusStop);

										}
									}
								}
							}

							SetClickEventFunc(ClickEvent);
							hMap = ShowMapForm(MODE_NORMAL, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 14, _flt, _flg);
						}
					}
					else if (lpnmhdr->code == NM_RCLICK)
					{
						_LISTGETITEM(hListView[0], l, 2, tmp, sizeof(tmp));

						_rClkEvent.type = 1;
						_rClkEvent.nID = atoi(tmp);

						HMENU pop = CreatePopupMenu();
						POINT pt;
	
						if (_rClkEvent.nID > 0)
						{
							AppendMenu(pop, MF_STRING, IDC_BT_ADD_FAVORITE, "즐겨찾기 추가");

							GetCursorPos(&pt);
							TrackPopupMenu(pop, TPM_LEFTALIGN, pt.x, pt.y, 0, hDlg, NULL);
							DestroyMenu(pop);
						}
					}
				}
				else if (lpnmhdr->hwndFrom == hListView[1]) //버스
				{
					if (lpnmhdr->code == NM_DBLCLK && !BusStopMode)
					{
						BusStopMode = 1;

						_LISTGETITEM(hListView[1], l, 2, tmp, sizeof(tmp));
						i = atoi(tmp);
						j = 0;

						PickBus = i;

						if (recvBus = FindBusToUpDown(i, 0))
						{
							TabControl_AddItem(hBusTab, 0, recvBus->BUSENDNAME, 1);
							++j;
						}

						if (recvBus = FindBusToUpDown(i, 1))
						{
							TabControl_AddItem(hBusTab, 1, recvBus->BUSENDNAME, 1);
							++j;
						}

						if (j > 0)
						{
							TabControl_SetFullSize(hBusTab, j);

							ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
							ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);

							ShowGroupsWindow(hDlg, BUSTAB_CT_MIN, BUSTAB_CT_MAX, SW_SHOW);

							ShowWindow(hTab, SW_HIDE);
							ShowWindow(hBusTab, SW_SHOW);

							TabCtrl_SetCurSel(hBusTab, 0);

							BUS_TO_LIST_STOP(hListView[2], PickBus, 0);
						}
					}
					else if (lpnmhdr->code == NM_RCLICK)
					{
						_LISTGETITEM(hListView[1], l, 2, tmp, sizeof(tmp));

						_rClkEvent.type = 0;
						_rClkEvent.nID = atoi(tmp);

						HMENU pop = CreatePopupMenu();
						POINT pt;
						BUS *recv;


						if (_rClkEvent.nID > 0)
						{
							if (recv = FindBusToUpDown(_rClkEvent.nID, 0))
							{
								sprintf(tmp, "즐겨찾기 추가(%s 방면)", recv->BUSENDNAME);
								AppendMenu(pop, MF_STRING, IDC_BT_ADD_FAVORITE_DOWN, tmp);
							}

							if (recv = FindBusToUpDown(_rClkEvent.nID, 1))
							{
								sprintf(tmp, "즐겨찾기 추가(%s 방면)", recv->BUSENDNAME);
								AppendMenu(pop, MF_STRING, IDC_BT_ADD_FAVORITE_UP, tmp);
							}

							GetCursorPos(&pt);
							TrackPopupMenu(pop, TPM_LEFTALIGN, pt.x, pt.y, 0, hDlg, NULL);
							DestroyMenu(pop);
						}
					}
				}
				else if (lpnmhdr->hwndFrom == hListView[2]) //버스->정류장뷰
				{
					if (lpnmhdr->code == NM_DBLCLK && BusStopMode)
					{
						i = TabCtrl_GetCurSel(hBusTab);
						SetClickEventFunc(ClickEvent);

						PickUpDown = i;

						recvBus = FindBusID(PickBus, i);
						DaejeonBusFormProc(NULL, WM_USER + 0xCF, NULL, (LPARAM)FindBusID(PickBus, i));
					}
				}
				else if (lpnmhdr->hwndFrom == hListView[3]) // 즐겨찾기
				{
					if (lpnmhdr->code == NM_DBLCLK )
					{
						_LISTGETITEM(hListView[3], l, 1, tmp, sizeof(tmp));

						switch(*tmp - '0')
						{ 
							case 0:
							{
								if (!BusStopMode)
								{
									BusStopMode = 2;

									_LISTGETITEM(hListView[3], l, 2, tmp, sizeof(tmp));
									k = *tmp == 'U';
									i = atoi(tmp + 1);

									PickBus = i;
									j = 0;

									if (recvBus = FindBusToUpDown(i, 0))
									{
										TabControl_AddItem(hBusTab, 0, recvBus->BUSENDNAME, 1);
										++j;
									}

									if (recvBus = FindBusToUpDown(i, 1))
									{
										TabControl_AddItem(hBusTab, 1, recvBus->BUSENDNAME, 1);
										++j;
									}

									if (j > 0)
									{
										TabControl_SetFullSize(hBusTab, j);

										ShowGroupsWindow(hDlg, IDC_LC_FAVORITE_LIST, IDC_LC_FAVORITE_LIST, SW_HIDE);
										ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
										ShowGroupsWindow(hDlg, BUSSTOP_CT_MIN, BUSSTOP_CT_MAX, SW_HIDE);
										ShowGroupsWindow(hDlg, BUS_CT_MIN, BUS_CT_MAX, SW_HIDE);

										ShowGroupsWindow(hDlg, BUSTAB_CT_MIN, BUSTAB_CT_MAX, SW_SHOW);

										ShowWindow(hTab, SW_HIDE);
										ShowWindow(hBusTab, SW_SHOW);

										TabCtrl_SetCurSel(hBusTab, k);

										BUS_TO_LIST_STOP(hListView[2], PickBus, k);
									}
								}

								break;
							}
							case 1:
							{
								_LISTGETITEM(hListView[3], l, 2, tmp, sizeof(tmp));

								if (recvBusStop = FindBusStopUID(atoi(tmp)))
								{
									ClearMarkUp();

									_flt = recvBusStop->latitude;
									_flg = recvBusStop->longitude;

									SetMarkUp(2, _flt, _flg, 0, 0, 4, 0x00FF00, recvBusStop);

									SetClickEventFunc(ClickEvent);
									hMap = ShowMapForm(MODE_NORMAL, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 14, _flt, _flg);
								}

								break;
							}
						}
					}
					else if (lpnmhdr->code == NM_RCLICK)
					{
						_LISTGETITEM(hListView[3], l, 1, tmp, sizeof(tmp));
						_rClkEvent.type = atoi(tmp);

						_LISTGETITEM(hListView[3], l, 2, tmp, sizeof(tmp));

						if (!_rClkEvent.type)
						{
							_rClkEvent.updown = *tmp == 'U';
							_rClkEvent.nID = atoi(tmp + 1);
						}
						else
						{
							_rClkEvent.nID = atoi(tmp);
						}

						HMENU pop = CreatePopupMenu();
						POINT pt;

						if (_rClkEvent.nID > 0)
						{
							AppendMenu(pop, MF_STRING, IDC_BT_DEL_FAVORITE, "즐겨찾기 삭제");

							GetCursorPos(&pt);
							TrackPopupMenu(pop, TPM_LEFTALIGN, pt.x, pt.y, 0, hDlg, NULL);
							DestroyMenu(pop);
						}
					}
				}
			}

			break;
		case WM_USER + 0xCF:
		{
			if (recvBus = (BUS*)lParam)
			{
				if (!recvBus->cntBstop)
				{
					MessageBox(hDlg, "해당 버스는 경유하는 정류장이 없습니다.", "알림", MB_ICONINFORMATION);
					return (INT_PTR)FALSE;
				}

				ClearMarkUp();

				DrawRoute(recvBus, NULL, NULL);
				DrawBus(BusImage, recvBus);
				DrawStop(recvBus, &_flt, &_flg);

				hMap = ShowMapForm(MODE_NORMAL, CW_USEDEFAULT, CW_USEDEFAULT, 700, 700, 12, _flt, _flg);
				InvalidateRgn(hMap, NULL, FALSE);
			}
			break;
		}
		case WM_MEASUREITEM:
		{
			((LPMEASUREITEMSTRUCT)lParam)->itemHeight = 50;
			return TRUE;
		}
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lDraw = (LPDRAWITEMSTRUCT)lParam;

			if (lDraw->CtlType == ODT_LISTVIEW)
			{
				RECT lRow = lDraw->rcItem;
				UINT lItem = lDraw->itemID;
				UINT lItemData = lDraw->itemData;
				HDC hdc = lDraw->hDC;
				HDC hMemDC;
				HFONT hOldFont;
				HBITMAP hOldBitmap;
				RECT prc;
				
				if (lDraw->itemState & ODS_SELECTED)
				{
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					SetTextColor(hdc, GetSysColor(COLOR_WINDOW));
					FillRect(hdc, &lRow, (HBRUSH)GetSysColorBrush(COLOR_HIGHLIGHT));
				}
				else
				{
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					FillRect(hdc, &lRow, (HBRUSH)GetSysColorBrush(COLOR_WINDOW));
				}

				char buffer[200] = { 0 };
				BOOL Stop = FALSE;
				INT xOriginSrc = 0;

				ListView_GetItemText(lDraw->hwndItem, lItem, 0, buffer, sizeof(buffer));
				ListView_GetSubItemRect(lDraw->hwndItem, lItem, 0, LVIR_BOUNDS, &prc);
				prc.right = ListView_GetColumnWidth(lDraw->hwndItem, 0);

				if (!strncmp(buffer, "1", 1))
				{
					Stop = TRUE;
				}
				else if (!strncmp(buffer, "2", 1))
				{
					xOriginSrc = 32;
					Stop = TRUE;
				}
				else if (!strncmp(buffer, "3", 1))
				{
					xOriginSrc = 64;
					Stop = TRUE;
				}

				if (Stop)
				{
					hMemDC = CreateCompatibleDC(hdc);
					hOldBitmap = (HBITMAP)SelectObject(hMemDC, (HBITMAP)BusImage);
					TransparentBlt(hdc, (prc.right - prc.left) / 2 - 16, (prc.bottom + prc.top) / 2 - 16, 32, 32, hMemDC, xOriginSrc, 0, 32, 32, 0xFF00FF);
					SelectObject(hMemDC, hOldBitmap);
					DeleteDC(hMemDC);
				}

				ListView_GetItemText(lDraw->hwndItem, lItem, 1, buffer, sizeof(buffer));
				ListView_GetSubItemRect(lDraw->hwndItem, lItem, 1, LVIR_BOUNDS, &prc);

				if (!Stop)
				{
					DrawText(hdc, buffer, strlen(buffer), &prc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
				}
				else
				{
					LONG originalbottom = prc.bottom;
					prc.bottom = (prc.top + prc.bottom) / 2;
					DrawText(hdc, buffer, strlen(buffer), &prc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

					prc.top = prc.bottom;
					prc.bottom = originalbottom;
					hOldFont = (HFONT)SelectObject(hdc, hBusFont);

					ListView_GetItemText(lDraw->hwndItem, lItem, 2, buffer, sizeof(buffer));
					DrawText(hdc, buffer, strlen(buffer), &prc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					SelectObject(hdc, hOldFont);
				}
			}

			return TRUE;
		}
		case WM_CLOSE:
			DeleteObject((HBITMAP)BusImage);
			DeleteObject((HBITMAP)hBusFont);
			ReleaseDB();
			ReleaseSeek();
			PostQuitMessage(0);
			break;
	}

	return (INT_PTR)FALSE;
}

VOID CALLBACK ClickEvent(HWND hWnd, BYTE Mode, INT nIndex, DWORD lParam, LPVOID lparg)
{
	LPARAM p_result;
	BUS *_p_recv, *_p_search;

	switch (Mode)
	{
		case MODE_NORMAL:
		{
			StopInfoFormProc(NULL, WM_USER + 0xCC, (WPARAM)lParam, (LPARAM)lparg);

			if (p_result = (LPARAM)DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DL_BUSSTOPINFO), hWnd, StopInfoFormProc))
			{
				_p_recv = (BUS*)p_result;

				PickBus = _p_recv->id;
				PickUpDown = _p_recv->Up;

				TabCtrl_SetCurSel(hBusTab, PickUpDown);

				if (!PickUpDown)
				{
					if (_p_search = FindBusToUpDown(PickBus, 1))
					{
						TabControl_AddItem(hBusTab, 0, _p_recv->BUSENDNAME, 1);
						TabControl_AddItem(hBusTab, 1, _p_search->BUSENDNAME, 1);
					}
				}
				else
				{
					if (_p_search = FindBusToUpDown(PickBus, 0))
					{
						TabControl_AddItem(hBusTab, 0, _p_search->BUSENDNAME, 1);
						TabControl_AddItem(hBusTab, 1, _p_recv->BUSENDNAME, 1);
					}
				}

				DaejeonBusFormProc(NULL, WM_USER + 0xCF, NULL, p_result);
			}


			break;
		}
	}
}
#endif