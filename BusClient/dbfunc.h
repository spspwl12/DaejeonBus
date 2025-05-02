#ifndef DBFUNC_H
#define DBFUNC_H

#include <Windows.h>

#define BUSSTOP_PATH "DATA\\DB\\busstop.txt"
#define BUS_PATH "DATA\\DB\\busno.txt"
#define ROUTE_PATH "DATA\\DB\\route.txt"
#define BUSROUTE_PATH "DATA\\DB\\busroute.txt"
#define FAVORITE_PATH "DATA\\DB\\favorite.txt"

typedef struct _BUS BUS;

//virtual
typedef struct _VirtualBusStop
{
	BUS *StopBus;
	UINT BusCarType;
	UINT BusCarNum;
}VBS;

typedef struct _BUSSTOP
{
	UINT stopno;
	UINT busid;
	double latitude;
	double longitude;
	char STOPNAME[50];
	// bus
	VBS *__vbs;
	UINT __vbscnt;
	struct _BUSSTOP *next;
}BUSSTOP;

typedef struct _BUS
{
	BOOL Up;
	UINT id;
	UINT nID;
	BYTE Type;
	CHAR BUSENDNAME[50];
	// BusStop
	BUSSTOP **p_BusStop;
	UINT cntBstop;
	// BusRoute
	UINT *p_BusRoute;
	UINT cntRoute;
	struct _BUS *next;
}BUS;

typedef struct _ROUTE
{
	UINT rid;
	UINT rcnt;
	double *_Lat;
	double *_Lng;
	struct _ROUTE *next;
}ROUTE;

typedef struct _FVR
{
	UINT type;
	UINT UpDown;
	UINT nID;
	struct _FVR *next;
}FAVORITE;

UINT			BUSSTOP_COUNT = 0;
UINT			BUS_COUNT = 0;
UINT			FAVORITE_COUNT = 0;

BUSSTOP			*_BSPST = NULL, *_BSPET = NULL;
BUS				*_BUSST = NULL, *_BUSET = NULL;
ROUTE			*_ROTST = NULL, *_ROTET = NULL;
FAVORITE		*_FAVST = NULL, *_FAVET = NULL;

LPCSTR			TypeName[] = { "급행", "", "", "", "마을", "첨단", "", "", "" };

BUSSTOP			*FindBusStopUID(UINT);
BUSSTOP			*FindBusStopID(UINT);
VOID			RemoveCRLF(LPSTR);

void LOAD_BUSSTOP_DB()
{
	HANDLE hFile;
	BUSSTOP *_newStop;
	char buf[100];
	int IsFirst = 1;
	char *St, *Ed, *rTn, nIdx = 0;
	int Pos = 0, Size;
	DWORD nBytesRead;

	if(hFile = CreateFile(BUSSTOP_PATH, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		Size = GetFileSize(hFile, NULL);

		while(Pos < Size)
		{
			if (!ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL))
				break;

			if (nBytesRead != sizeof(buf))
				memset(buf + nBytesRead, 0, sizeof(buf) - nBytesRead);

			Pos += sizeof(buf);

			if (IsFirst)
			{
				_BSPST = (BUSSTOP*)malloc(sizeof(BUSSTOP));
				_BSPET = _BSPST;
				_BSPET->next = NULL;
				_newStop = _BSPET;
				IsFirst = 0;
			}
			else
				_newStop = (BUSSTOP*)malloc(sizeof(BUSSTOP));

			St = buf;
			if (!(rTn = strchr(buf, 10)))
				rTn = buf + nBytesRead;
			Ed = strchr(buf, ',');
			nIdx = 0;

			while (Ed && St < rTn)
			{
				*Ed = 0;

				switch (nIdx)
				{
					case 0:
						_newStop->stopno = atoi(St);
						break;
					case 1:
						_newStop->busid = atoi(St);
						break;
					case 2:
						_newStop->latitude = atof(St);
						break;
					case 3:
						_newStop->longitude = atof(St);
						break;
					case 4:
						RemoveCRLF(St);
						strcpy(_newStop->STOPNAME, St);
						break;
				}

				++nIdx;
				St = Ed + 1;
				Ed = strchr(St, ',');

				if (!Ed || Ed > rTn)
					Ed = rTn;
			}

			Pos -= sizeof(buf) - (rTn - buf) - 1;
			SetFilePointer(hFile, Pos, NULL, FILE_BEGIN);

			_BSPET->next = _newStop;
			_BSPET = _newStop;
			_newStop->next = NULL;
			//virtual
			_newStop->__vbs = NULL;
			_newStop->__vbscnt = 0;
			++BUSSTOP_COUNT;
		}

		CloseHandle(hFile);
	}
}


void LOAD_BUS_DB()
{
	HANDLE hFile;
	BUS *_newBus;
	BUSSTOP *_p_bufStop;

	char buf[1000];
	int IsFirst = 1;
	char *St, *Ed, *rTn, nIdx = 0;
	int Pos = 0, Size;
	DWORD nBytesRead;

	if (hFile = CreateFile(BUS_PATH, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		Size = GetFileSize(hFile, NULL);

		while (Pos < Size)
		{
			if (!ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL))
				break;

			if (nBytesRead != sizeof(buf))
				memset(buf + nBytesRead, 0, sizeof(buf) - nBytesRead);

			Pos += sizeof(buf);

			if (IsFirst)
			{
				_BUSST = (BUS*)malloc(sizeof(BUS));
				_BUSET = _BUSST;
				_BUSET->next = NULL;
				_newBus = _BUSET;
				IsFirst = 0;
			}
			else
				_newBus = (BUS*)malloc(sizeof(BUS));

			_newBus->cntBstop = 0;
			_newBus->cntRoute = 0;
			_newBus->p_BusStop = NULL;
			_newBus->p_BusRoute = NULL;

			St = buf;
			if (!(rTn = strchr(buf, 10)))
				rTn = buf + nBytesRead;
			Ed = strchr(buf, ',');
			nIdx = 0;

			while (Ed && St < rTn && nIdx >= 0)
			{
				*Ed = 0;

				switch (nIdx)
				{
					case 0:
						_newBus->Up = *St == 'U';
						break;
					case 1:
						_newBus->id = atoi(St);
						break;
					case 2:
						_newBus->nID = atoi(St);
						break;
					case 3:
						_newBus->Type = atoi(St);
						break;
					case 4:
						strcpy(_newBus->BUSENDNAME, St);
						break;
					default:
						if (_p_bufStop = *St == 'U' ? FindBusStopUID(atoi(St + 1)) : FindBusStopID(atoi(St)))
						{
							if (!_newBus->cntBstop)
								_newBus->p_BusStop = (BUSSTOP**)malloc(sizeof(BUSSTOP*));
							else
								_newBus->p_BusStop = (BUSSTOP**)realloc(_newBus->p_BusStop, sizeof(BUSSTOP*) * (_newBus->cntBstop + 1));

							_newBus->p_BusStop[_newBus->cntBstop] = _p_bufStop;
							_newBus->cntBstop++;
						}
						break;
				}

				++nIdx;
				St = Ed + 1;
				Ed = strchr(St, ',');

				if (!Ed || Ed > rTn)
					Ed = rTn;
	
			}

			Pos -= sizeof(buf) - (rTn - buf) - 1;
			SetFilePointer(hFile, Pos, NULL, FILE_BEGIN);

			_BUSET->next = _newBus;
			_BUSET = _newBus;
			_newBus->next = NULL;
			++BUS_COUNT;
		}

		CloseHandle(hFile);
	}
}


void LOAD_BUSROUTE_DB()
{
	HANDLE hFile;
	BUS *_currentBus = _BUSST;
	char buf[10000];
	int IsFirst = 1;
	char *St, *Ed, *rTn;
	int nIdx = 0;
	int Pos = 0, Size;
	DWORD nBytesRead;

	if (hFile = CreateFile(BUSROUTE_PATH, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		Size = GetFileSize(hFile, NULL);

		while (Pos < Size && _currentBus)
		{
			if (!ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL))
				break;

			if (nBytesRead != sizeof(buf))
				memset(buf + nBytesRead, 0, sizeof(buf) - nBytesRead);

			Pos += sizeof(buf);

			St = buf;
			if (!(rTn = strchr(buf, 10)))
				rTn = buf + nBytesRead;
			Ed = strchr(buf, ',');
			nIdx = 0;

			while (Ed && St < rTn && nIdx >= 0)
			{
				*Ed = 0;

				switch (nIdx)
				{
					case 0:
						if (!(_currentBus->Up && *St == 'U') && !(!_currentBus->Up && *St != 'U')) nIdx = -1;
						break;
					case 1:
						if (_currentBus->id != atoi(St)) nIdx = -1;
						break;
					case 2:
						if (_currentBus->nID != atoi(St)) nIdx = -1;
						break;
					case 3:
						if (_currentBus->Type != atoi(St)) nIdx = -1;
						break;
					default:
						if (nIdx > 3)
						{
							if (nIdx == 4)
								_currentBus->p_BusRoute = (UINT*)malloc(sizeof(UINT));
							else
								_currentBus->p_BusRoute = (UINT*)realloc(_currentBus->p_BusRoute, sizeof(UINT) * (nIdx - 3));

							_currentBus->p_BusRoute[nIdx - 4] = atoi(St);
							_currentBus->cntRoute++;
						}
						break;
				}

				if (nIdx < 0)
				{
					_currentBus = _currentBus->next;

					if (!_currentBus)
					{
						CloseHandle(hFile);
						return;
					}

					SetFilePointer(hFile, Pos - sizeof(buf), NULL, FILE_BEGIN);

					if (!ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL))
						break;

					St = buf;
					rTn = strchr(buf, 10);
					Ed = strchr(buf, ',');
					nIdx = 0;

					continue;
				}
				else
					++nIdx;

				St = Ed + 1;
				Ed = strchr(St, ',');

				if (!Ed || Ed > rTn)
					Ed = rTn;
			}

			Pos -= sizeof(buf) - (rTn - buf) - 1;
			SetFilePointer(hFile, Pos, NULL, FILE_BEGIN);

			_currentBus = _currentBus->next;
		}

		CloseHandle(hFile);
	}
}

void LOAD_ROUTE_DB()
{
	HANDLE hFile;
	ROUTE *_newRoute;
	char buf[9000];
	int IsFirst = 1;
	char *St, *Ed, *rTn;
	int nIdx = 0;
	int Pos = 0, Size;
	DWORD nBytesRead;

	if (hFile = CreateFile(ROUTE_PATH, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		Size = GetFileSize(hFile, NULL);

		while (Pos < Size)
		{
			if (!ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL))
				break;

			if (nBytesRead != sizeof(buf))
				memset(buf + nBytesRead, 0, sizeof(buf) - nBytesRead);

			Pos += sizeof(buf);

			if (IsFirst)
			{
				_ROTST = (ROUTE*)malloc(sizeof(ROUTE));
				_ROTET = _ROTST;
				_ROTET->next = NULL;
				_newRoute = _ROTET;
				IsFirst = 0;
			}
			else
				_newRoute = (ROUTE*)malloc(sizeof(ROUTE));

			_newRoute->rcnt = 0;
			_newRoute->_Lat = NULL;
			_newRoute->_Lng = NULL;

			St = buf;
			if (!(rTn = strchr(buf, 10)))
				rTn = buf + nBytesRead;
			Ed = strchr(buf, ',');
			nIdx = 0;

			while (Ed && St < rTn && nIdx >= 0)
			{
				*Ed = 0;

				switch (nIdx)
				{
					case 0:
						_newRoute->rid = atoi(St);
						break;
					case 1:
						break;
					default:
						if (nIdx > 1)
						{
							if (nIdx > 3)
							{
								if(nIdx % 2)
									_newRoute->_Lng = (double*)realloc(_newRoute->_Lng, sizeof(double) * ((nIdx - 1) / 2));
								else
									_newRoute->_Lat = (double*)realloc(_newRoute->_Lat, sizeof(double) * (nIdx / 2));
							}
							else if (nIdx == 2)
								_newRoute->_Lat = (double*)malloc(sizeof(double));
							else if (nIdx == 3)
								_newRoute->_Lng = (double*)malloc(sizeof(double));


							if (nIdx % 2)
								_newRoute->_Lng[((nIdx - 1) / 2) - 1] = atof(St);
							else 
								_newRoute->_Lat[(nIdx / 2) - 1] = atof(St);


							_newRoute->rcnt++;
						}
						break;
				}

				++nIdx;
				St = Ed + 1;
				Ed = strchr(St, ',');

				if (!Ed || Ed > rTn)
					Ed = rTn;
			}

			Pos -= sizeof(buf) - (rTn - buf) - 1;
			SetFilePointer(hFile, Pos, NULL, FILE_BEGIN);

			_ROTET->next = _newRoute;
			_ROTET = _newRoute;
			_newRoute->next = NULL;
		}

		CloseHandle(hFile);
	}
}

void LOAD_FAVORITE_DB()
{
	HANDLE hFile;
	FAVORITE *_newFavorite;
	char buf[100];
	int IsFirst = 1;
	char *St, *Ed, *rTn;
	int nIdx = 0;
	int Pos = 0, Size;
	DWORD nBytesRead;

	if (hFile = CreateFile(FAVORITE_PATH, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		Size = GetFileSize(hFile, NULL);

		while (Pos < Size)
		{
			if (!ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL))
				break;

			if (nBytesRead != sizeof(buf))
				memset(buf + nBytesRead, 0, sizeof(buf) - nBytesRead);

			Pos += sizeof(buf);

			if (IsFirst)
			{
				_FAVST = (FAVORITE*)malloc(sizeof(FAVORITE));
				_FAVET = _FAVST;
				_FAVET->next = NULL;
				_newFavorite = _FAVET;
				IsFirst = 0;
			}
			else
				_newFavorite = (FAVORITE*)malloc(sizeof(FAVORITE));

			St = buf;
			if (!(rTn = strchr(buf, 10)))
				rTn = buf + nBytesRead;
			Ed = strchr(buf, ',');
			nIdx = 0;

			while (Ed && St < rTn && nIdx >= 0)
			{
				*Ed = 0;

				switch (nIdx)
				{
					case 0:
						_newFavorite->type = atoi(St);
						break;
					case 1:
						if (!_newFavorite->type)
						{
							_newFavorite->UpDown = *St == 'U';
							_newFavorite->nID = atoi(St + 1);
						}
						else
						{
							_newFavorite->nID = atoi(St);
						}
						break;
				}

				++nIdx;
				St = Ed + 1;
				Ed = strchr(St, ',');

				if (!Ed || Ed > rTn)
					Ed = rTn;
			}

			Pos -= sizeof(buf) - (rTn - buf) - 1;
			SetFilePointer(hFile, Pos, NULL, FILE_BEGIN);

			_FAVET->next = _newFavorite;
			_FAVET = _newFavorite;
			_newFavorite->next = NULL;
			++FAVORITE_COUNT;
		}

		CloseHandle(hFile);
	}
}

void SAVE_FAVORITE_DB()
{
	HANDLE hFile;
	FAVORITE *_next = _FAVST;
	CHAR lpszbuf[100] = { 0 };
	DWORD nBytesWritten;

	if (hFile = CreateFile(FAVORITE_PATH, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		while (_next)
		{
			if (!_next->type)
			{
				sprintf_s(lpszbuf, sizeof(lpszbuf), "0,%c%d\r\n", _next->UpDown ? 'U' : 'D', _next->nID);
			}
			else
			{
				sprintf_s(lpszbuf, sizeof(lpszbuf), "1,%d\r\n", _next->nID);
			}

			WriteFile(hFile, lpszbuf, strlen(lpszbuf), &nBytesWritten, NULL);
			_next = _next->next;
		}

		CloseHandle(hFile);
	}
}

BOOL ADD_FAVORITE(UINT type, UINT UpDown, UINT nID)
{
	FAVORITE *_newFavorite;
	FAVORITE *next = _FAVST;
	BOOL FirstCreate = FALSE;

	FirstCreate = next ? FALSE : TRUE;

	while (next)
	{
		if (type == 1 && next->nID == nID)
			return 0;
		else if (type == 0 && next->nID == nID && next->UpDown == UpDown)
			return 0;

		next = next->next;
	}

	_newFavorite = (FAVORITE*)malloc(sizeof(FAVORITE));
	_newFavorite->UpDown = UpDown;
	_newFavorite->nID = nID;
	_newFavorite->type = type;

	if (FirstCreate)
		_FAVST = _FAVET = _newFavorite;

	_FAVET->next = _newFavorite;
	_FAVET = _newFavorite;
	_newFavorite->next = NULL;

	++FAVORITE_COUNT;
	return 1;
}

BOOL DEL_FAVORITE(UINT type, UINT UpDown, UINT nID)
{
	FAVORITE *next = _FAVST;
	FAVORITE *prev = NULL;

	while (next)
	{
		if ((type == 1 && next->nID == nID) || (type == 0 && next->nID == nID && next->UpDown == UpDown))
		{
			if (!prev) // 처음
			{
				if (!next->next)
				{
					_FAVST = NULL;
					_FAVET = NULL;
				}
				else
					_FAVST = next->next;
			}
			else
			{
				if (!next->next)
				{
					prev->next = NULL;
					_FAVET = prev;
				}
				else
					prev->next = next->next;
			}

			free(next);
			return TRUE;
		}

		prev = next;
		next = next->next;
	}

	return FALSE;
}


//.......................
BUSSTOP *FindBusStopUID(UINT BusID)
{
	BUSSTOP *next = _BSPST;

	while (next)
	{
		if (next->busid == BusID)
			return next;

		next = next->next;
	}
	return NULL;
}

BUSSTOP *FindBusStopID(UINT StopID)
{
	BUSSTOP *next = _BSPST;

	while (next)
	{
		if (next->stopno == StopID)
			return next;

		next = next->next;
	}
	return NULL;
}

BUS *FindBusID(UINT uBusID, BOOL _Up)
{
	BUS *next = _BUSST;

	while (next)
	{
		if (next->id == uBusID && next->Up == _Up)
			return next;

		next = next->next;
	}
	return NULL;
}


ROUTE *FindRouteID(UINT RouteID)
{
	ROUTE *next = _ROTST;

	while (next)
	{
		if (next->rid == RouteID)
			return next;

		next = next->next;
	}
	return NULL;
}

BUS *FindBusToUpDown(UINT BusID, BOOL Up)
{
	BUS *next = _BUSST;

	while (next)
	{
		if (next->id == BusID && next->Up == Up)
			return next;

		next = next->next;
	}
	return NULL;
}

VOID RemoveCRLF(LPSTR str)
{
	for (UINT i = 0; i < strlen(str); ++i)
	{
		if (str[i] == 10 || str[i] == 13)
			str[i] = 0;
	}
}

LPCSTR CovBusType(BYTE Type)
{
	if (Type >= 1 && Type <= 9)
		return TypeName[Type - 1];
	else
		return NULL;
}

VOID ReleaseDB()
{
	BUSSTOP *next = _BSPST;
	BUS *_next = _BUSST;
	ROUTE *__next = _ROTST;
	FAVORITE *___next = _FAVST;
	void *p_next;

	while (next)
	{
		p_next = next->next;
		free(next);
		next = (BUSSTOP*)p_next;
	}

	while (_next)
	{
		p_next = _next->next;
		if (_next->p_BusStop)
			free(_next->p_BusStop);
		if (_next->p_BusRoute)
			free(_next->p_BusRoute);
		free(_next);
		_next = (BUS*)p_next;
	}

	while (__next)
	{
		p_next = __next->next;
		if (__next->_Lat)
			free(__next->_Lat);
		if (__next->_Lng)
			free(__next->_Lng);
		free(__next);
		__next = (ROUTE*)p_next;
	}

	while (___next)
	{
		p_next = ___next->next;
		free(___next);
		___next = (FAVORITE*)p_next;
	}
}
#endif