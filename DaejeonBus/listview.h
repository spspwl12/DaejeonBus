#include <Windows.h>
#include <CommCtrl.h>
#include <stdarg.h>
#include "dbfunc.h"

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

VOID _SELLIST(HWND hListView, UINT nIndex)
{
	LV_ITEM lvi;

	SetFocus(hListView);
	memset(&lvi, 0, sizeof(lvi));

	lvi.mask = LVIF_STATE;
	lvi.iItem = nIndex;
	lvi.iSubItem = 0;
	lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
	lvi.cchTextMax = 0;
	lvi.pszText = NULL;
	lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;

	SendMessage(hListView, LVM_SETITEMSTATE, nIndex, (LPARAM)&lvi);
	ListView_EnsureVisible(hListView, nIndex, TRUE);
}

INT _SEARCHLIST(HWND hListView, UINT nCol_Index, LPSTR cmp, INT Previous = 0)
{
	int i, j;
	char tmp[50];
	LVITEM lvi = { 0 };

	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 50;
	lvi.iSubItem = nCol_Index;
	lvi.pszText = tmp;

	j = SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);

	for (i = Previous; i < j; ++i)
	{
		lvi.iItem = i;
		SendMessage(hListView, LVM_GETITEMTEXT, i, (LPARAM)&lvi);

		if (!strncmp(cmp, tmp, strlen(cmp)))
			return i;
	}

	return -1;
}

VOID _SWAPLIST(HWND hListView, UINT nIndex1, UINT nIndex2, UINT nCol_Min, UINT nCol_Max, UINT uSize)
{
	LVITEM lvi1 = { 0 };
	LVITEM lvi2 = { 0 };

	char tmp1[100], tmp2[100];

	lvi1.mask = LVIF_TEXT;
	lvi1.cchTextMax = uSize;
	lvi1.iItem = nIndex1;
	lvi1.pszText = tmp1;

	lvi2.mask = LVIF_TEXT;
	lvi2.cchTextMax = uSize;
	lvi2.iItem = nIndex2;
	lvi2.pszText = tmp2;

	for (UINT i = nCol_Min; i <= nCol_Max; ++i)
	{
		lvi1.iSubItem = i;
		lvi2.iSubItem = i;

		SendMessage(hListView, LVM_GETITEMTEXT, nIndex1, (LPARAM)&lvi1);
		SendMessage(hListView, LVM_GETITEMTEXT, nIndex2, (LPARAM)&lvi2);

		SendMessage(hListView, LVM_SETITEMTEXT, nIndex2, (LPARAM)&lvi1);
		SendMessage(hListView, LVM_SETITEMTEXT, nIndex1, (LPARAM)&lvi2);
	}
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

VOID _DELLISTFORID(HWND hListView, UINT nCol_Index, UINT nID)
{
	int Index;
	char tmp[50] = { 0 };

	sprintf(tmp, "%d", nID);

	if ((Index = _SEARCHLIST(hListView, nCol_Index, tmp)) >= 0)
		SendMessage(hListView, LVM_DELETEITEM, Index, 0);
}

VOID _DELLISTFORSTR(HWND hListView, UINT nCol_Index, LPSTR cmp)
{
	int Index;

	if ((Index = _SEARCHLIST(hListView, nCol_Index, cmp)) >= 0)
		SendMessage(hListView, LVM_DELETEITEM, Index, 0);
}

VOID _DELLISTFORIDUP(HWND hListView, UINT nID, BOOL Up)
{
	int Index = 0;
	char tmp[50] = { 0 };

	while (Index != -1)
	{
		sprintf(tmp, "%d", nID);

		if ((Index = _SEARCHLIST(hListView, 0, tmp, Index)) >= 0)
		{
			_LISTGETITEM(hListView, Index, 4, tmp, 2);

			if (Up == (*tmp - '0'))
			{
				SendMessage(hListView, LVM_DELETEITEM, Index, 0);
				return;
			}

			++Index;
		}
	}
}

INT _SEARCHLISTFORIDUP(HWND hListView, UINT nID, BOOL Up)
{
	int Index = 0;
	char tmp[50] = { 0 };

	while (Index != -1)
	{
		sprintf(tmp, "%d", nID);

		if ((Index = _SEARCHLIST(hListView, 0, tmp, Index)) >= 0)
		{
			_LISTGETITEM(hListView, Index, 4, tmp, 2);

			if (Up == (*tmp - '0'))
			{
				return Index;
			}

			++Index;
		}
	}

	return -1;
}

UINT DB_TO_LIST_TO_BUSSTOP(HWND hListView)
{
	BUSSTOP *_bNext = _BSPST;
	char buf[50];
	int i = 0;

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

	return i;
}

UINT DB_TO_LIST_TO_BUS(HWND hListView)
{
	BUS *_bNext = _BUSST;
	int i = 0;

	_CLSLIST(hListView);

	while (_bNext)
	{
		_ADDLISTBATCH(hListView, i, 5, 0, "dssdd", &_bNext->id, _bNext->BUSENDNAME, CovBusType(_bNext->Type), &_bNext->nID, &_bNext->Up);
		++i;
		_bNext = _bNext->next;
	}

	return i;

}

UINT DB_TO_LIST_TO_BUSSTOP_SIMPLE(HWND hListView)
{
	BUSSTOP *_bNext = _BSPST;
	char buf[50];
	int i = 0;

	_CLSLIST(hListView);

	while (_bNext)
	{
		if (_bNext->stopno == 0)
			sprintf(buf, "U%d", _bNext->busid);
		else
			sprintf(buf, "%d", _bNext->stopno);

		_ADDLISTBATCH(hListView, i, 3, 0, "ssd", buf, _bNext->STOPNAME, &_bNext->busid);
		++i;
		_bNext = _bNext->next;
	}

	return i;
}

VOID SEARCH_TO_LIST_FOR_BUSSTOP(HWND hListView, HWND NoticeStatic, UINT Type, LPSTR SearchValue)
{
	BUSSTOP *_bNext = _BSPST;
	char buf[100];
	int i = 0;

	_CLSLIST(hListView);

	switch (Type)
	{
		case 1:
		{
			while (_bNext)
			{
				if (strstr(_bNext->STOPNAME, SearchValue))
				{
					if (_bNext->stopno == 0)
						sprintf(buf, "U%d", _bNext->busid);
					else
						sprintf(buf, "%d", _bNext->stopno);

					_ADDLISTBATCH(hListView, i, 5, 0, "ssffd", buf, _bNext->STOPNAME, &_bNext->latitude, &_bNext->longitude, &_bNext->busid);
					++i;
				}

				_bNext = _bNext->next;
			}

			break;
		}
		case 2:
		{
			while (_bNext)
			{
				if (_bNext->stopno == atoi(SearchValue) || (_bNext->stopno == 0 && _bNext->busid == atoi(SearchValue)))
				{
					if (_bNext->stopno == 0)
						sprintf(buf, "U%d", _bNext->busid);
					else
						sprintf(buf, "%d", _bNext->stopno);

					_ADDLISTBATCH(hListView, i, 5, 0, "ssffd", buf, _bNext->STOPNAME, &_bNext->latitude, &_bNext->longitude, &_bNext->busid);
					++i;
				}

				_bNext = _bNext->next;
			}
			break;
		}
	}

	sprintf(buf, "총 %d 개가 검색되었음.", i);
	SendMessage(NoticeStatic, SB_SETTEXT, 0, (LPARAM)buf);
}


VOID SEARCH_TO_LIST_FOR_BUSSTOP_SIMPLE(HWND hListView, UINT Type, LPSTR SearchValue)
{
	BUSSTOP *_bNext = _BSPST;
	char buf[100];
	int i = 0;

	_CLSLIST(hListView);

	switch (Type)
	{
		case 1:
		{
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

			break;
		}
		case 2:
		{
			while (_bNext)
			{
				if (_bNext->stopno == atoi(SearchValue) || (_bNext->stopno == 0 && _bNext->busid == atoi(SearchValue)))
				{
					_bNext->stopno ? sprintf(buf, "%d", _bNext->stopno) : sprintf(buf, "U%d", _bNext->busid);
					_ADDLISTBATCH(hListView, i, 3, 0, "ssd", buf, _bNext->STOPNAME, &_bNext->busid);
					++i;
				}

				_bNext = _bNext->next;
			}
			break;
		}
	}
}

VOID SEARCH_TO_LIST_FOR_BUS(HWND hListView, HWND NoticeStatic, UINT Type, LPSTR SearchValue)
{
	BUS *_bNext = _BUSST;
	char buf[100];
	int i = 0;

	_CLSLIST(hListView);

	// 노선 방면 종류

	switch (Type)
	{
		case 1:
		{
			while (_bNext)
			{
				sprintf_s(buf, sizeof(buf), "%d", _bNext->nID);

				if (strstr(buf, SearchValue))
				{
					_ADDLISTBATCH(hListView, i, 5, 0, "dssdd", &_bNext->id, _bNext->BUSENDNAME, CovBusType(_bNext->Type), &_bNext->nID, &_bNext->Up);
					++i;
				}

				_bNext = _bNext->next;
			}

			break;
		}
		case 2:
		{
			while (_bNext)
			{
				if (strstr(_bNext->BUSENDNAME, SearchValue))
				{
					_ADDLISTBATCH(hListView, i, 5, 0, "dssdd", &_bNext->id, _bNext->BUSENDNAME, CovBusType(_bNext->Type), &_bNext->nID, &_bNext->Up);
					++i;
				}

				_bNext = _bNext->next;
			}

			break;
		}
		case 3:
		{
			while (_bNext)
			{
				if (strstr(CovBusType(_bNext->Type), SearchValue))
				{
					_ADDLISTBATCH(hListView, i, 5, 0, "dssdd", &_bNext->id, _bNext->BUSENDNAME, CovBusType(_bNext->Type), &_bNext->nID, &_bNext->Up);
					++i;
				}

				_bNext = _bNext->next;
			}
			break;
		}
	}

	sprintf(buf, "총 %d 개가 검색되었음.", i);
	SendMessage(NoticeStatic, SB_SETTEXT, 0, (LPARAM)buf);
}

VOID BUS_TO_LIST(HWND hList, UINT BusStopID)
{
	BUS *_sNext = _BUSST;
	UINT i;
	char tmp[50], *Cov;

	SendMessage(hList, LB_RESETCONTENT, 0, 0);

	while (_sNext)
	{
		for (i = 0; i < _sNext->cntBstop; ++i)
		{
			if (_sNext->p_BusStop[i] && _sNext->p_BusStop[i]->busid == BusStopID)
			{
				Cov = (LPSTR)CovBusType(_sNext->Type);
				!Cov || !strlen(Cov) ? sprintf(tmp, "%d번 (%s 방면)", _sNext->nID, _sNext->BUSENDNAME) : sprintf(tmp, "%s %d번 (%s 방면)", Cov, _sNext->nID, _sNext->BUSENDNAME);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)tmp);
				break;
			}
		}

		_sNext = _sNext->next;
	}
}

BUS *LIST_TO_BUS(UINT BusStopID, UINT nIndex)
{
	BUS *_sNext = _BUSST;
	UINT i, j = 0;

	while (_sNext)
	{
		for (i = 0; i < _sNext->cntBstop; ++i)
		{
			if (_sNext->p_BusStop[i] && _sNext->p_BusStop[i]->busid == BusStopID)
			{
				if( j == nIndex )
					return _sNext;

				++j;
				break;
			}
		}
		_sNext = _sNext->next;
	}

	return NULL;
}

VOID BUS_TO_LISTVIEW(HWND hList, UINT BusStopID)
{
	BUS *_sNext = _BUSST;
	UINT i, j;
	char tmp[50];

	_CLSLIST(hList);

	j = 0;

	while (_sNext)
	{
		for (i = 0; i < _sNext->cntBstop; ++i)
		{
			if (_sNext->p_BusStop[i] && _sNext->p_BusStop[i]->busid == BusStopID)
			{
				sprintf(tmp, "%c%d", _sNext->Up ? 'U' : 'D', _sNext->id);
				_ADDLISTBATCH(hList, j, 4, 0, "ssds", _sNext->BUSENDNAME, CovBusType(_sNext->Type), &_sNext->nID, tmp);

				++j;
				break;
			}
		}

		_sNext = _sNext->next;
	}
}

VOID BUSSTOP_TO_LISTVIEW(HWND hList, UINT BusID, BOOL Up)
{
	BUS *_sNext = _BUSST;
	UINT i, j, k;
	char tmp[50] = { 0 };

	_CLSLIST(hList);

	j = 1;

	while (_sNext)
	{
		if (_sNext->id == BusID && _sNext->Up == Up)
		{
			for (i = 0; i < _sNext->cntBstop; ++i)
			{
				if (_sNext->p_BusStop[i])
				{
					if (k = _sNext->p_BusStop[i]->stopno)
					{
						_ADDLISTBATCH(hList, i, 4, 0, "ddsd", &j, &k, _sNext->p_BusStop[i]->STOPNAME, &_sNext->p_BusStop[i]->busid);
					}
					else
					{
						sprintf(tmp, "U%d", _sNext->p_BusStop[i]->busid);
						_ADDLISTBATCH(hList, i, 4, 0, "dssd", &j, tmp, _sNext->p_BusStop[i]->STOPNAME, &_sNext->p_BusStop[i]->busid);
					}

					++j;
				}
			}

			return;
		}

		_sNext = _sNext->next;
	}
}
