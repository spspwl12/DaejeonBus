#include <Windows.h>
#include <CommCtrl.h>
#include <stdarg.h>
#include "dbfunc.h"
#include "virtualBus.h"

VOID _ADDCOL(HWND hListView, UINT nIndex, UINT nWidth, LPCSTR lpszText)
{
	LVCOLUMN lvcol = { 0 };

	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = nWidth;
	lvcol.pszText = (LPSTR)lpszText;
	lvcol.iSubItem = 0;

	SendMessage(hListView, LVM_INSERTCOLUMN, (WPARAM)nIndex, (LPARAM)&lvcol);
}
// 일괄적으로 리스트뷰에 아이템을 추가하는 함수
VOID _ADDLISTBATCH(HWND hListView, UINT nIndex, UINT nHdrCnt, BOOL modify, LPCSTR _ListAddType, ...)
{
	LVITEM lvi = { 0 };

	va_list arg_ptr;
	void *p;
	char *sz;
	UINT i;
	char tmp[50] = { 0 };

	lvi.mask = LVIF_TEXT;
	lvi.iItem = nIndex;

	va_start(arg_ptr, _ListAddType);
	sz = (char*)_ListAddType;
	i = 0;

	while (*sz && nHdrCnt > i)
	{
		lvi.iSubItem = i;

		switch (*sz)
		{
			case 's':
			{
				if (p = va_arg(arg_ptr, char*))
				{
					lvi.pszText = (LPSTR)p;
					SendMessage(hListView, LVM_SETITEMTEXT, (WPARAM)nIndex, (LPARAM)&lvi);

					if (!modify)
						SendMessage(hListView, LVM_INSERTITEM, 0, (LPARAM)&lvi);
				}

				break;
			}
			case 'd':
			{
				if (p = va_arg(arg_ptr, int*))
				{
					sprintf(tmp, "%d", *(int*)p);
					lvi.pszText = tmp;
					SendMessage(hListView, LVM_SETITEMTEXT, (WPARAM)nIndex, (LPARAM)&lvi);

					if (!modify)
						SendMessage(hListView, LVM_INSERTITEM, 0, (LPARAM)&lvi);
				}
				break;
			}
			case 'f':
			{
				if (p = va_arg(arg_ptr, double*))
				{
					sprintf(tmp, "%f", *(double*)p);
					lvi.pszText = tmp;
					SendMessage(hListView, LVM_SETITEMTEXT, (WPARAM)nIndex, (LPARAM)&lvi);

					if (!modify)
						SendMessage(hListView, LVM_INSERTITEM, 0, (LPARAM)&lvi);
				}
				break;
			}
		}

		++sz;
		++i;
	}

	va_end(arg_ptr);
}

VOID _LISTGETITEM(HWND hList, UINT nIndex, UINT nCol_Index, LPSTR lpszText, int cchMax)
{
	LVITEM lvi = { 0 };

	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = cchMax;
	lvi.iSubItem = nCol_Index;
	lvi.iItem = nIndex;
	lvi.pszText = lpszText;

	SendMessage(hList, LVM_GETITEMTEXT, nIndex, (LPARAM)&lvi);
}

VOID _CLSLIST(HWND hListView)
{
	SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
}

UINT DB_TO_LIST_TO_BUSSTOP(HWND hListView)
{
	BUSSTOP *_bNext = _BSPST;
	char buf[50];
	int i = 0;

	ShowWindow(hListView, SW_HIDE);

	_CLSLIST(hListView);

	while (_bNext)
	{
		if (_bNext->stopno == 0)
			sprintf(buf, "U%d", _bNext->busid);
		else
			sprintf(buf, "%d", _bNext->stopno);

		_ADDLISTBATCH(hListView, i, 5, 0, "ssffd", buf, _bNext->STOPNAME, &_bNext->latitude, &_bNext->longitude, &_bNext->busid);
		++i;
		_bNext = _bNext->next;
	}

	ShowWindow(hListView, SW_SHOW);

	return i;
}


UINT DB_TO_LIST_TO_BUS(HWND hListView)
{
	BUS *_bNext = _BUSST;
	BUS *_FindDown;
	int i = 0;
	char tmp[50];
	char tmp2[100];

	ShowWindow(hListView, SW_HIDE);

	_CLSLIST(hListView);

	while (_bNext)
	{
		if (_bNext->Up)
		{
			sprintf(tmp, "%s %d", CovBusType(_bNext->Type), _bNext->nID);
			if (_FindDown = FindBusToUpDown(_bNext->id, 0))
				sprintf(tmp2, "%s <-> %s", _FindDown->BUSENDNAME, _bNext->BUSENDNAME);
			else
				sprintf(tmp2, "%s", _bNext->BUSENDNAME);
			_ADDLISTBATCH(hListView, i, 3, 0, "ssd", tmp, tmp2, &_bNext->id);
			++i;
		}

		_bNext = _bNext->next;
	}

	ShowWindow(hListView, SW_SHOW);

	return i;

}


UINT DB_TO_LIST_TO_FAVORITE(HWND hListView)
{
	FAVORITE *_bNext = _FAVST;
	BUS *recvBus;
	BUSSTOP *recvBusStop;
	char tmp[50];
	char tmp2[50];
	int i = 0;

	ShowWindow(hListView, SW_HIDE);

	_CLSLIST(hListView);

	while (_bNext)
	{
		if (!_bNext->type)
		{
			if (recvBus = FindBusID(_bNext->nID, _bNext->UpDown))
			{
				sprintf(tmp, "버스 : %s %d 번(%s 방면)", CovBusType(recvBus->Type), recvBus->nID, recvBus->BUSENDNAME);
				sprintf(tmp2, "%c%d", _bNext->UpDown ? 'U' : 'D', _bNext->nID);
			}
		}
		else
		{
			if (recvBusStop = FindBusStopUID(_bNext->nID))
			{
				sprintf(tmp, "정류장 : %s", recvBusStop->STOPNAME);
				sprintf(tmp2, "%d", _bNext->nID);
			}
		}

		_ADDLISTBATCH(hListView, i, 3, 0, "sds", tmp, &_bNext->type, tmp2);

		++i;
		_bNext = _bNext->next;
	}

	ShowWindow(hListView, SW_SHOW);

	return i;

}

VOID SEARCH_TO_LIST_FOR_BUSSTOP(HWND hListView, LPSTR SearchValue)
{
	BUSSTOP *_bNext = _BSPST;
	char buf[100];
	int i = 0;

	_CLSLIST(hListView);

	if (!strlen(SearchValue))
		return;

	ShowWindow(hListView, SW_HIDE);

	while (_bNext)
	{
		if (strstr(_bNext->STOPNAME, SearchValue))
		{
			if (_bNext->stopno == 0)
				sprintf(buf, "U%d", _bNext->busid);
			else
				sprintf(buf, "%d", _bNext->stopno);

			_ADDLISTBATCH(hListView, i, 3, 0, "ssd", buf, _bNext->STOPNAME, &_bNext->busid);
			++i;
		}

		_bNext = _bNext->next;
	}

	ShowWindow(hListView, SW_SHOW);
}


VOID SEARCH_TO_LIST_FOR_BUS(HWND hListView, LPSTR SearchValue)
{
	BUS *_bNext = _BUSST;
	char buf[100];
	BUS *_FindDown;
	int i = 0;
	char tmp[50];
	char tmp2[100];

	ShowWindow(hListView, SW_HIDE);

	_CLSLIST(hListView);

	while (_bNext)
	{
		sprintf_s(buf, sizeof(buf), "%d", _bNext->nID);

		if (_bNext->Up && strstr(buf, SearchValue))
		{
			sprintf(tmp, "%s %d", CovBusType(_bNext->Type), _bNext->nID);
			if (_FindDown = FindBusToUpDown(_bNext->id, 0))
				sprintf(tmp2, "%s <-> %s", _FindDown->BUSENDNAME, _bNext->BUSENDNAME);
			else
				sprintf(tmp2, "%s", _bNext->BUSENDNAME);
			_ADDLISTBATCH(hListView, i, 3, 0, "ssd", tmp, tmp2, &_bNext->id);

			++i;
		}

		_bNext = _bNext->next;
	}

	ShowWindow(hListView, SW_SHOW);
}

VOID BUS_TO_LIST(HWND hListView, UINT BusStopID)
{
	BUS *_sNext = _BUSST;
	UINT i, j, k;
	char tmp[3][50];

	_CLSLIST(hListView);

	j = 0;

	while (_sNext)
	{
		for (i = 0; i < _sNext->cntBstop; ++i)
		{
			if (_sNext->p_BusStop[i] && _sNext->p_BusStop[i]->busid == BusStopID)
			{
				sprintf_s(tmp[0], sizeof(tmp[0]), "%s%d", CovBusType(_sNext->Type), _sNext->nID);

				if( (k = CalcBusTime(_sNext, _sNext->p_BusStop[i], i) ) == -1 )
					sprintf_s(tmp[1], sizeof(tmp[1]), "운행중X");
				else if(k == 0)
					sprintf_s(tmp[1], sizeof(tmp[1]), "진입");
				else if(k==1)
					sprintf_s(tmp[1], sizeof(tmp[1]), "전정류장출발");
				else
					sprintf_s(tmp[1], sizeof(tmp[1]), "%d 분", k);

				sprintf_s(tmp[2], sizeof(tmp[2]), "%c%d", _sNext->Up ? 'U' : 'D', _sNext->id);

				_ADDLISTBATCH(hListView, j, 4, 0, "ssss", tmp[0], tmp[1], _sNext->BUSENDNAME, tmp[2]);
				++j;
				break;
			}
		}

		_sNext = _sNext->next;
	}
}

VOID BUS_TO_LIST_STOP(HWND hListView, UINT BusID, BOOL Up)
{
	BUS *_bNext = _BUSST;
	UINT i, j = 0, k = 0;
	UINT l = 0;
	VBS *vbs;
	char tmp[20];

	ShowWindow(hListView, SW_HIDE);

	_CLSLIST(hListView);

	while (_bNext)
	{
		if (_bNext->id == BusID && _bNext->Up == Up)
		{
			for (i = 0; i < _bNext->cntBstop; ++i)
			{
				if (_bNext->p_BusStop[i])
				{
					vbs = _bNext->p_BusStop[i]->__vbs;

					for (k = 0, l = 0; k < _bNext->p_BusStop[i]->__vbscnt; ++k)
					{
						if (vbs[k].StopBus == _bNext)
						{
							sprintf_s(tmp, sizeof(tmp), "대전 75자 %d", vbs[k].BusCarNum);
							_ADDLISTBATCH(hListView, i, 3, 0, "dss", &vbs[k].BusCarType, _bNext->p_BusStop[i]->STOPNAME, tmp);
							l = 1;
						}
					}

					if(!l)
						_ADDLISTBATCH(hListView, i, 3, 0, "dss", &j, _bNext->p_BusStop[i]->STOPNAME, "");

				}
			}

		}

		_bNext = _bNext->next;
	}

	ShowWindow(hListView, SW_SHOW);
}