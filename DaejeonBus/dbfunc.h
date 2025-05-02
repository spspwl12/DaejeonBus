#ifndef DBFUNC_H
#define DBFUNC_H

#include <Windows.h>

#define BUSSTOP_PATH "DATA\\DB\\busstop.txt"
#define BUS_PATH "DATA\\DB\\busno.txt"
#define ROUTE_PATH "DATA\\DB\\route.txt"
#define BUSROUTE_PATH "DATA\\DB\\busroute.txt"

typedef struct _BUSSTOP
{
	UINT stopno;
	UINT busid;
	double latitude;
	double longitude;
	char STOPNAME[50];
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

UINT			BUSSTOP_COUNT = 0;
UINT			BUS_COUNT = 0;
BUSSTOP			*_BSPST = NULL, *_BSPET = NULL;
BUS				*_BUSST = NULL, *_BUSET = NULL;
ROUTE			*_ROTST = NULL, *_ROTET = NULL;
LPCSTR			TypeName[] = { "급행", "도시", "광역", "외곽", "마을", "첨단", "광역", "광역", "광역" };

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

void SAVE_BUSSTOP_DB()
{
	HANDLE hFile;
	BUSSTOP *_nextStop = _BSPST;
	CHAR lpszbuf[500] = { 0 };
	DWORD nBytesWritten;

	if (hFile = CreateFile(BUSSTOP_PATH, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		while (_nextStop)
		{
			memset(lpszbuf, 0, sizeof(lpszbuf));

			sprintf(lpszbuf, "%d,%d,%f,%f,%s\r\n",
				_nextStop->stopno,
				_nextStop->busid,
				_nextStop->latitude,
				_nextStop->longitude,
				_nextStop->STOPNAME
			);

			WriteFile(hFile, lpszbuf, strlen(lpszbuf), &nBytesWritten, NULL);
			_nextStop = _nextStop->next;
		}

		CloseHandle(hFile);
	}
}

void SAVE_BUS_DB()
{
	HANDLE hFile;
	BUS *_nextBus = _BUSST;
	CHAR lpszbuf[500] = { 0 };
	DWORD nBytesWritten;
	UINT BusStopID, nCnt;

	if (hFile = CreateFile(BUS_PATH, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		while (_nextBus)
		{
			sprintf_s(lpszbuf, sizeof(lpszbuf), "%s,%d,%d,%d,%s",
				_nextBus->Up ? "UP" : "DOWN",
				_nextBus->id,
				_nextBus->nID,
				_nextBus->Type,
				_nextBus->BUSENDNAME
			);

			WriteFile(hFile, lpszbuf, strlen(lpszbuf), &nBytesWritten, NULL);

			if (nCnt = _nextBus->cntBstop)
			{
				WriteFile(hFile, ",", 1, &nBytesWritten, NULL);

				for (UINT i = 0; i < nCnt; ++i)
				{
					if( BusStopID = _nextBus->p_BusStop[i]->stopno )
						sprintf_s(lpszbuf, sizeof(lpszbuf), "%d", BusStopID);
					else
						sprintf_s(lpszbuf, sizeof(lpszbuf), "U%d", _nextBus->p_BusStop[i]->busid);

					WriteFile(hFile, lpszbuf, strlen(lpszbuf), &nBytesWritten, NULL);

					if( i < nCnt - 1 )
						WriteFile(hFile, ",", 1, &nBytesWritten, NULL);
					else
						WriteFile(hFile, "\r\n", 2, &nBytesWritten, NULL);
				}
			}
			else
				WriteFile(hFile, "\r\n", 2, &nBytesWritten, NULL);

			_nextBus = _nextBus->next;
		}

		CloseHandle(hFile);
	}
}

void SAVE_BUSROUTE()
{
	HANDLE hFile;
	BUS *_nextBus = _BUSST;
	CHAR lpszbuf[500] = { 0 };
	DWORD nBytesWritten;
	UINT nCnt;

	if (hFile = CreateFile(BUSROUTE_PATH, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0))
	{
		while (_nextBus)
		{
			sprintf_s(lpszbuf, sizeof(lpszbuf), "%s,%d,%d,%d",
				_nextBus->Up ? "UP" : "DOWN",
				_nextBus->id,
				_nextBus->nID,
				_nextBus->Type
			);

			WriteFile(hFile, lpszbuf, strlen(lpszbuf), &nBytesWritten, NULL);

			if (nCnt = _nextBus->cntRoute)
			{
				WriteFile(hFile, ",", 1, &nBytesWritten, NULL);

				for (UINT i = 0; i < nCnt; ++i)
				{
					sprintf_s(lpszbuf, sizeof(lpszbuf), "%d", _nextBus->p_BusRoute[i]);
					WriteFile(hFile, lpszbuf, strlen(lpszbuf), &nBytesWritten, NULL);

					if (i < nCnt - 1)
						WriteFile(hFile, ",", 1, &nBytesWritten, NULL);
					else
						WriteFile(hFile, "\r\n", 2, &nBytesWritten, NULL);
				}
			}
			else
				WriteFile(hFile, "\r\n", 2, &nBytesWritten, NULL);

			_nextBus = _nextBus->next;
		}

		CloseHandle(hFile);
	}
}

BOOL ADD_BUSSTOP_IN_BUS(BUS *_p_Bus, UINT uBusID)
{
	int Index;

	if (_p_Bus)
	{
		Index = _p_Bus->cntBstop;

		if(!Index)
			_p_Bus->p_BusStop = (BUSSTOP**)malloc(sizeof(BUSSTOP*));
		else
			_p_Bus->p_BusStop = (BUSSTOP**)realloc(_p_Bus->p_BusStop, sizeof(BUSSTOP*) * (Index + 1));

		_p_Bus->p_BusStop[Index] = FindBusStopUID(uBusID);
		_p_Bus->cntBstop++;

		if (_p_Bus->p_BusStop[Index])
			return TRUE;
	}

	return FALSE;
}

BOOL DEL_BUSSTOP_IN_BUS(UINT uBusStopID, UINT uBusID, BOOL _Up)
{
	int Index, cnt;
	int j;
	BUS *next = _BUSST;

	while (next)
	{
		if (next->id == uBusID && next->Up == _Up)
		{
			cnt = next->cntBstop;

			if (cnt == 0)
				return FALSE;
			else if (cnt == 1)
			{
				free(next->p_BusStop);
				next->p_BusStop = NULL;
				next->cntBstop = 0;
				return TRUE;
			}
			else if (cnt > 1)
			{
				for (Index = 0; Index < cnt; ++Index)
				{
					if (next->p_BusStop[Index] && next->p_BusStop[Index]->busid == uBusStopID)
					{
						for (j = Index; j < cnt - 1; ++j)
							next->p_BusStop[j] = next->p_BusStop[j + 1];

						next->p_BusStop = (BUSSTOP**)realloc(next->p_BusStop, sizeof(BUSSTOP*) * (cnt - 1));
						next->cntBstop--;

						return TRUE;
					}
				}
			}

			break;
		}
		next = next->next;
	}

	return FALSE;
}


BOOL ADD_BUSSTOP(UINT BusStopNum, UINT BusStopID, double _lati, double _lon, LPSTR BusStopName)
{
	BUSSTOP *_newStop;
	BUSSTOP *next = _BSPST;
	BOOL FirstCreate = FALSE;

	FirstCreate = next ? FALSE : TRUE;

	while (next)
	{
		if (next->busid == BusStopID)
			return 0;

		next = next->next;
	}

	_newStop = (BUSSTOP*)malloc(sizeof(BUSSTOP));
	_newStop->stopno = BusStopNum;
	_newStop->busid = BusStopID;
	_newStop->latitude = _lati;
	_newStop->longitude = _lon;
	memcpy(_newStop->STOPNAME, BusStopName, 50);

	if (FirstCreate)
		_BSPST = _BSPET = _newStop;

	_BSPET->next = _newStop;
	_BSPET = _newStop;
	_newStop->next = NULL;

	++BUSSTOP_COUNT;
	return 1;
}


BOOL MODIFY_BUSSTOP(UINT BusStopNum, UINT BusStopID, double _lati, double _lon, LPSTR BusStopName)
{
	BUSSTOP *next = _BSPST;

	while (next)
	{
		if (next->busid == BusStopID)
		{
			next->busid = BusStopID;
			next->stopno = BusStopNum;
			next->latitude = _lati;
			next->longitude = _lon;
			memcpy(next->STOPNAME, BusStopName, 50);

			return TRUE;
		}

		next = next->next;
	}

	return FALSE;
}

BOOL DEL_BUSSTOP(UINT BusStopID)
{
	BUSSTOP *next = _BSPST;
	BUSSTOP *prev = NULL;
	BUS *next_b = _BUSST;
	int i, j, k;

	// 버스에 종속된 정류장을 제거하는 작업
	while (next_b)
	{
		if (j = next_b->cntBstop)
		{
			if (j == 1)
			{
				if (next_b->p_BusStop[0] && next_b->p_BusStop[0]->busid == BusStopID)
				{
					free(next_b->p_BusStop);
					next_b->p_BusStop = NULL;
					next_b->cntBstop = 0;

					goto NEXT_STEP;
				}
			}
			else if (j > 1)
			{
				for (i = 0; i < j; ++i)
				{
					if (next_b->p_BusStop[i] && next_b->p_BusStop[i]->busid == BusStopID)
					{
						for (k = i; k < j - 1; ++k)
							next_b->p_BusStop[k] = next_b->p_BusStop[k + 1];

						next_b->p_BusStop = (BUSSTOP**)realloc(next_b->p_BusStop, sizeof(BUSSTOP*) * (j - 1));
						next_b->cntBstop--;

						break;
					}
				}
			}
		}

		next_b = next_b->next;
	}

NEXT_STEP:
	// 버스 정류장 데이터가 들어있는 링크드 리스트에서 할당해제 시키는 source
	while (next)
	{
		if (next->busid == BusStopID)
		{
			if (!prev) // 처음
			{
				if (!next->next)
				{
					_BSPST = NULL;
					_BSPET = NULL;
				}
				else
					_BSPST = next->next;
			}
			else
			{
				if (!next->next)
				{
					prev->next = NULL;
					_BSPET = prev;
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

BOOL ADD_BUS_ROUTE(BUS *_p_Bus, UINT RouteID)
{
	int i, k;

	if (_p_Bus)
	{
		if ((k = _p_Bus->cntRoute) && _p_Bus->p_BusRoute)
		{
			for (i = 0; i < k; ++i)
			{
				if (_p_Bus->p_BusRoute[i] == RouteID)
					return FALSE;
			}

			_p_Bus->p_BusRoute = (UINT*)realloc(_p_Bus->p_BusRoute, sizeof(UINT) * (k + 1));
		}
		else
		{
			k = 0;
			_p_Bus->p_BusRoute = (UINT*)malloc(sizeof(UINT));
		}

		_p_Bus->p_BusRoute[k] = RouteID;
		++_p_Bus->cntRoute;
	}

	return TRUE;
}

BOOL DEL_BUS_ROUTE(BUS *_p_Bus, UINT RouteID)
{
	int i, j, k;
	if (_p_Bus)
	{
		if (k = _p_Bus->cntRoute)
		{
			if (k == 1 && _p_Bus->p_BusRoute[0] && _p_Bus->p_BusRoute[0] == RouteID)
			{
				free(_p_Bus->p_BusRoute);
				_p_Bus->p_BusRoute = NULL;
				_p_Bus->cntRoute = 0;

				return TRUE;
			}
			else if(k > 1)
			{
				for (i = 0; i < k; ++i)
				{
					if (_p_Bus->p_BusRoute[i] == RouteID)
					{
						for (j = i; j < k - 1; ++j)
							_p_Bus->p_BusRoute[j] = _p_Bus->p_BusRoute[j + 1];

						_p_Bus->p_BusRoute = (UINT*)realloc(_p_Bus->p_BusRoute, sizeof(UINT) * (k - 1));
						_p_Bus->cntRoute--;

						return TRUE;
						break;
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL ADD_BUS(UINT BusID, BOOL UpDown, UINT BusRoute, UINT BusType, LPSTR BusEndName)
{
	BUS *_newBus;
	BUS *next = _BUSST;
	BOOL FirstCreate = FALSE;

	FirstCreate = next ? FALSE : TRUE;

	while (next)
	{
		if (next->id == BusID && next->Up == UpDown)
			return 0;
		next = next->next;
	}

	_newBus = (BUS*)malloc(sizeof(BUS));
	_newBus->id = BusID;
	_newBus->nID = BusRoute;
	_newBus->Type = BusType;
	_newBus->Up = UpDown;
	_newBus->cntBstop = 0;
	_newBus->cntRoute = 0;
	_newBus->p_BusRoute = NULL;
	_newBus->p_BusStop = NULL;
	memcpy(_newBus->BUSENDNAME, BusEndName, 50);

	if (FirstCreate)
		_BUSST = _BUSET = _newBus;

	_BUSET->next = _newBus;
	_BUSET = _newBus;
	_newBus->next = NULL;

	++BUS_COUNT;
	return 1;
}

BOOL MODIFY_BUS(UINT BusID, BOOL UpDown, UINT BusRoute, UINT BusType, LPSTR BusEndName)
{
	BUS *next = _BUSST;

	while (next)
	{
		if (next->id == BusID && next->Up == UpDown)
		{
			next->id = BusID;
			next->nID = BusRoute;
			next->Up = UpDown;
			next->Type = BusType;
			memcpy(next->BUSENDNAME, BusEndName, 50);

			return TRUE;
		}

		next = next->next;
	}

	return FALSE;
}

BOOL DEL_BUS(UINT BusID, BOOL Up)
{
	BUS *next = _BUSST;
	BUS *prev = NULL;

	while (next)
	{
		if (next->id == BusID && next->Up == Up)
		{
			if (!prev) // 처음
			{
				if (!next->next)
				{
					_BUSST = NULL;
					_BUSET = NULL;
				}
				else
					_BUSST = next->next;
			}
			else
			{
				if (!next->next)
				{
					prev->next = NULL;
					_BUSET = prev;
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

VOID RemoveCRLF(LPSTR str)
{
	for (UINT i = 0; i < strlen(str); ++i)
	{
		if (str[i] == 10 || str[i] == 13)
			str[i] = 0;
	}
}

VOID SwapBusStop(BUS *_p_Bus, UINT Index1, UINT Index2)
{
	UINT i;
	void *p;

	if (_p_Bus && _p_Bus->p_BusStop && (i = _p_Bus->cntBstop))
	{
		if (Index1 < i && Index2 < i)
		{
			p = _p_Bus->p_BusStop[Index2];
			_p_Bus->p_BusStop[Index2] = _p_Bus->p_BusStop[Index1];
			_p_Bus->p_BusStop[Index1] = (BUSSTOP*)p;
		}
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
}
#endif