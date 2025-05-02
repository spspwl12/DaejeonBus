#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>
#include <stdarg.h>

VOID ShowGroupsWindow(HWND DialogHandle, UINT MIN_CTRLID, UINT MAX_CTRLID, INT nCmdShow)
{
	for (UINT i = MIN_CTRLID; i <= MAX_CTRLID; ++i)
		ShowWindow(GetDlgItem(DialogHandle, i), nCmdShow);
}

void TabControl_AddItem(HWND hTab, BYTE Index, LPCSTR Tab_Name, BOOL Set = 0)
{
	TCITEM tab_info;
	memset(&tab_info, 0, sizeof(tab_info));

	tab_info.mask = TCIF_TEXT;
	tab_info.pszText = (LPSTR)Tab_Name;
	tab_info.cchTextMax = strlen(Tab_Name);

	SendMessage(hTab, Set ? TCM_SETITEM : TCM_INSERTITEM, Index, (LPARAM)&tab_info);
}

void TabControl_SetFullSize(HWND hTab, INT Count)
{
	RECT rt;
	int item_width;
	GetClientRect(hTab, &rt);
	item_width = (rt.right / Count) - 5;

	TabCtrl_SetItemSize(hTab, item_width, 22);
}

BOOL GetDlgItemBatch(HWND hDlg, int *nIDDlgItems, int nCtrlCnt, LPCSTR szTypeStr, ...)
{
	va_list arg_ptr;
	void *p;
	char *sz;
	int i, len;

	va_start(arg_ptr, szTypeStr);
	
	sz = (char*)szTypeStr;

	i = 0;

	while(*sz && nCtrlCnt > i)
	{
		switch (*sz)
		{
			case 's':
			{
				if (p = va_arg(arg_ptr, char*))
				{
					if (!SendDlgItemMessage(hDlg, nIDDlgItems[i], WM_GETTEXTLENGTH, 0, 0))
						return FALSE;

					len = strtoul(sz + 1, &sz, 10);
					--sz;
					GetDlgItemText(hDlg, nIDDlgItems[i], (LPSTR)p, len);
				}
				break;
			}
			case 'd':
			{
				if (!SendDlgItemMessage(hDlg, nIDDlgItems[i], WM_GETTEXTLENGTH, 0, 0))
					return FALSE;
				if (p = va_arg(arg_ptr, int*))
					*(int*)p = GetDlgItemInt(hDlg, nIDDlgItems[i], NULL, FALSE);
				break;
			}
			case 'c':
			{
				if (p = va_arg(arg_ptr, int*))
					*(int*)p = SendDlgItemMessage(hDlg, nIDDlgItems[i], CB_GETCURSEL, 0, 0);
				break;
			}
			case 'f':
			{
				if (p = va_arg(arg_ptr, double*))
				{
					char tmp[50] = { 0 };
					GetDlgItemText(hDlg, nIDDlgItems[i], tmp, sizeof(tmp));
					*(double*)p = strtod(tmp, NULL);
				}
				break;
			}
		}

		++sz;
		++i;
	}

	va_end(arg_ptr);

	return TRUE;
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

VOID DrawBus(HBITMAP BusImage, BUS *recvBus)
{
	UINT i, j;
	BUSSTOP *recvBusStop;
	VBS *vbs;

	double _lt, _lg;

	if (!recvBus)
		return;

	for (i = 0; i < recvBus->cntBstop; ++i)
	{
		if (recvBusStop = recvBus->p_BusStop[i])
		{
			for (j = 0; j < recvBusStop->__vbscnt; ++j)
			{
				vbs = recvBusStop->__vbs;

				if (vbs[j].StopBus == recvBus)
				{
					_lt = recvBusStop->latitude;
					_lg = recvBusStop->longitude;

					switch (vbs[j].BusCarType)
					{
						case 1:
							SetMarkImage(BusImage, _lt, _lg, 0, 0);
							break;
						case 2:
							SetMarkImage(BusImage, _lt, _lg, 32, 0);
							break;
						case 3:
							SetMarkImage(BusImage, _lt, _lg, 64, 0);
							break;
					}
				}
			}
		}
	}
}

#endif