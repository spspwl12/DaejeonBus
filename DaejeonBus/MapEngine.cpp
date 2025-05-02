#include "MapEngine.h"
#include "resource.h"

// 좌표계 : WGS84 

struct
{
	BYTE Type;
	double lat;
	double lon;
	double lat2;
	double lon2;
	BYTE radius;
	COLORREF dwColor;
	// 0--
	LPVOID pMemo;
}__DrawMarkup[DRAW_MARK_MAX_COUNT];

HINSTANCE					hInst;
HWND						hMapWnd;
HBITMAP						_Bitmap[IMG_H][IMG_W];
SIZE						LHMAX;
SIZE						LHMIN;
LONG						ST_MAPX;
LONG						ST_MAPY;
LONG						ED_MAPX;
LONG						ED_MAPY;
LONG						ST_MAPH;
DWORD						ZoomSeek[10];
WORD						__DrawMarkupCnt = 0;
RECT						MapRect;
RECT						WndRect;
POINT						WndCenter;
POINT						__Increase;
BYTE						Zoom;
BYTE						Mode;
char						iAlpha;
POINT						ps_ckp;
POINT						ps_psz;
POINT						ps_nsz;
int							ClickState;
CLKEVENTPROC				__ClickEvent;
PMapHash					_p_MHash = NULL;
PMapROUTE					_p_Route = NULL;
PMapROUTE					_p_PickRoute = NULL;
DWORD						dwCntMHash = 0;
HFONT						MyFont;

VOID RegisterMapClass(HINSTANCE hInstance)
{
	WNDCLASS WndClass;

	hInst = hInstance;

	WndClass.cbClsExtra			= 0;
	WndClass.cbWndExtra			= 0;
	WndClass.hbrBackground		= (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor			= LoadCursor(hInst, MAKEINTRESOURCE(IDC_CU_DEFAULT));
	WndClass.hIcon				= LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance			= hInst;
	WndClass.lpfnWndProc		= (WNDPROC)__MapViewerWndProc;
	WndClass.lpszClassName		= "MapViewer";
	WndClass.lpszMenuName		= NULL;
	WndClass.style				= CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);
}

HWND ShowMapForm(BYTE _Mode, UINT x, UINT y, UINT cx, UINT cy, UINT __Zoom, double __Lat, double __Long)
{
	MapParameter _MP;
	RECT center;

	_MP._Mode = _Mode;

	if (!hMapWnd)
	{
		_MP.Lat = __Lat;
		_MP.Long = __Long;
		_MP.Zoom = __Zoom;

		GetClientRect(GetDesktopWindow(), &center);
		hMapWnd = CreateWindow("MapViewer", NULL, WS_OVERLAPPEDWINDOW, x, y, cx, cy, NULL, NULL, hInst, 0);
		SetWindowPos(hMapWnd, NULL, center.right / 2 - cx / 2, center.bottom / 2 - cy / 2, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		ShowWindow(hMapWnd, SW_SHOW);

		SendMessage(hMapWnd, MAP_INITDATA, 0, (LPARAM)&_MP);
	}
	else
	{
		SetForegroundWindow(hMapWnd);
		GetClientRect(hMapWnd, &WndRect);
		iAlpha = 0;

		if (Zoom != __Zoom)
		{
			CHANGEMAP(hMapWnd, __Zoom);
			Zoom = __Zoom;
		}

		SetLatLon(__Lat, __Long, Zoom, &ps_nsz);

		InvalidateRgn(hMapWnd, NULL, FALSE);
	}

	return hMapWnd;
}

LRESULT CALLBACK __MapViewerWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	PMapParameter _MP;

	int i, j;

	switch (iMessage)
	{
		case MAP_INITDATA:
			_MP = (PMapParameter)lParam;
			Zoom = _MP->Zoom;
			Mode = _MP->_Mode;
			InitMap(hWnd, Zoom);
			iAlpha = 0;
			GetClientRect(hWnd, &WndRect);
			SetLatLon(_MP->Lat, _MP->Long, Zoom, &ps_nsz);
			break;
		case MAP_WINDOWLIVE:
			return TRUE;
		case WM_SIZE:
			GetClientRect(hWnd, &WndRect);
			break;
		case WM_MOUSEWHEEL:
			if ((short)HIWORD(wParam) < 0)
			{
				if (--iAlpha <= -2)
				{
					iAlpha = 0;

					if (Zoom > 11)
					{
						iAlpha = 0;
						Zoom--;
						ps_nsz.x /= 2;
						ps_nsz.y /= 2;
						CHANGEMAP(hWnd, Zoom);
						InvalidateRgn(hWnd, NULL, FALSE);
					}
				}
			}
			else
			{
				if (++iAlpha >= 2)
				{
					iAlpha = 0;

					if (Zoom < 17)
					{
						Zoom++;
						ps_nsz.x *= 2;
						ps_nsz.y *= 2;
						CHANGEMAP(hWnd, Zoom);
						InvalidateRgn(hWnd, NULL, FALSE);
					}
				}
			}

			break;
		case WM_LBUTTONDOWN:
			ps_ckp.x = LOWORD(lParam);
			ps_ckp.y = HIWORD(lParam);
			ps_psz = ps_nsz;
			ClickState = 1;

			switch(Mode)
			{
				case MODE_NORMAL:
				{
					SetCapture(hWnd);
					SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_CU_DRAG)));

					if ((i = IsCursorInRect(lParam)) != -1 && __ClickEvent)
					{
						__ClickEvent(hWnd, Mode, i, lParam, __DrawMarkup[i].pMemo);
						ClickState = 0;
					}
					break;
				}
			}

			break;
		case WM_MOUSEMOVE:
			i = LOWORD(lParam);
			j = HIWORD(lParam);

			if (Mode == MODE_NORMAL && IsCursorInRect(lParam) != -1)
				SetCursor(LoadCursor(NULL, IDC_HAND));

			if (ClickState == 1)
			{
				if (Mode == MODE_PICK)
				{
					SetCapture(hWnd);
					SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_CU_DRAG)));
				}

				ps_nsz.x = (ps_ckp.x - i) + ps_psz.x;
				ps_nsz.y = (ps_ckp.y - j) + ps_psz.y;

				InvalidateRgn(hWnd, NULL, FALSE);
			}
			else 
			{
				if (Mode == MODE_ROUTE)
					DrawSearchRoute(lParam);
			}

			break;
		case WM_LBUTTONUP:
			ClickState = 0;

			switch (Mode)
			{
				case MODE_PICK:
				{
					if (ps_psz.x == ps_nsz.x && ps_psz.y == ps_nsz.y)
					{
						double d1, d2;

						ClearMarkUp();
						PickMap(lParam, &d1, &d2);
						SetMarkUp(2, d1, d2, 0, 0, 4, 0xFF, NULL);
						__ClickEvent(hWnd, Mode, 0, lParam, NULL);
						InvalidateRgn(hWnd, NULL, FALSE);
					}
					break;
				}
				case MODE_ROUTE:
				{
					if (_p_PickRoute && ps_psz.x == ps_nsz.x && ps_psz.y == ps_nsz.y)
					{
						__ClickEvent(hWnd, Mode, 0, _p_PickRoute->rid, NULL);
						InvalidateRgn(hWnd, NULL, FALSE);
					}
					break;
				}
			}

			ps_psz = ps_nsz;
			ReleaseCapture();
			break;
		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			DrawMap(hWnd, hDC, Zoom);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			hMapWnd = NULL;
			ReleaseCapture();
			DestroyWindow(hWnd);
			ReleaseData();
			ReleaseFont();
			break;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}


Gdiplus::Image *ext_png(LPCSTR path, DWORD dwSeek, DWORD dwFileSize)
{
	Gdiplus::Image				*Img = NULL;
	HANDLE						hFile = NULL;
	HGLOBAL						Buffer = NULL;
	void						*pDest = NULL;
	IStream						*pStream = NULL;
	DWORD						dwBytesRead;
	BOOL						_ERROR_SUCCESS = FALSE;

	if (hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
	{
		if (Buffer = GlobalAlloc(GMEM_MOVEABLE, dwFileSize))
		{
			if (pDest = GlobalLock(Buffer))
			{
				SetFilePointer(hFile, dwSeek, NULL, FILE_BEGIN);

				if (ReadFile(hFile, pDest, dwFileSize, &dwBytesRead, NULL))
				{
					if (CreateStreamOnHGlobal(Buffer, FALSE, &pStream) == S_OK)
						Img = Gdiplus::Image::FromStream(pStream);
					else 
						goto RELEASE;
				}
				else goto RELEASE;
			}
			else goto RELEASE;
		}
		else goto RELEASE;
	}
	else goto RELEASE;

	_ERROR_SUCCESS = TRUE;

RELEASE:
	if (!_ERROR_SUCCESS)
		MessageBox(0, "ext_png() Function Error!!", "Error", MB_ICONERROR);

	if (pStream)
		pStream->Release();
	if (pDest)
		GlobalUnlock(Buffer);
	if (Buffer)
		GlobalFree(Buffer);
	if (hFile)
		CloseHandle(hFile);

	return Img;
}

VOID SetZoom(BYTE _Zoom)
{
	ST_MAPX = FIRST_ZOOM_SW * (1 << (_Zoom - FIRST_ZOOM));
	ST_MAPY = FIRST_ZOOM_SH * (1 << (_Zoom - FIRST_ZOOM));
	ED_MAPX = FIRST_ZOOM_EW * (1 << (_Zoom - FIRST_ZOOM));
	ED_MAPY = FIRST_ZOOM_EH * (1 << (_Zoom - FIRST_ZOOM));
	ST_MAPH = ED_MAPY - ST_MAPY;
}

VOID SetLatLon(double _lat, double _long, BYTE _Zoom, LPPOINT lpp)
{
	double sy, x, y;

	sy = sin(_lat *  PI / 180);

	x = 256 * (0.5 + _long / 360);
	y = 256 * (0.5 - log((1 + sy) / (1 - sy)) / (4 * PI));

	lpp->x = (LONG)(x * (1 << _Zoom) - (ST_MAPX * 256));
	lpp->y = (LONG)(y * (1 << _Zoom) - (ST_MAPY * 256));
}

VOID SetLatLontoTileXY(double _lat, double _long, BYTE _Zoom, LPPOINT pTileIndex, LPPOINT pOffset)
{
	double sy, x, y;

	sy = sin(_lat *  PI / 180);

	x = 256 * (0.5 + _long / 360);
	y = 256 * (0.5 - log((1 + sy) / (1 - sy)) / (4 * PI));

	pTileIndex->x = (LONG)(x * (1 << _Zoom) / 256);
	pTileIndex->y = (LONG)(y * (1 << _Zoom) / 256);

	pOffset->x = (LONG)(x * (1 << _Zoom)) % 256;
	pOffset->y = (LONG)(y * (1 << _Zoom)) % 256;
}

VOID TileXYToLatLon(LPPOINT pTileIndex, LPPOINT pOffset, BYTE _Zoom, double *_lat, double *_long)
{
	double x, y;

	x = ((pTileIndex->x * 256.0) + (pOffset->x * 1.0)) / (1 << _Zoom);
	y = ((pTileIndex->y * 256.0) + (pOffset->y * 1.0)) / (1 << _Zoom);

	*_long = (x / 256 - 0.5) * 360;
	*_lat = asin(tanh((0.5 - (y / 256)) * (4 * PI) / 2)) * 180.0 / PI;
}

BOOL LoadSeek(LPCSTR path)
{
	if (_p_MHash)
		return TRUE;

	HANDLE hFile;
	DWORD z, sk, si, pvz = 0;
	DWORD dwFileSize, Pos, nBytesRead;

	char tmp[50];
	char *St, *rTn, *Ed, Indx;

	if( hFile = CreateFile(path, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0) )
	{
		dwFileSize = GetFileSize(hFile, NULL);
		Pos = 0;

		while(Pos < dwFileSize)
		{
			ReadFile(hFile, tmp, sizeof(tmp), &nBytesRead, NULL);
			Pos += sizeof(tmp);

			St = tmp;
			rTn = strchr(tmp, 10);
			Ed = strchr(St, ' ');
			Indx = 0;

			while (Ed && St < rTn)
			{
				*Ed = 0;

				switch (Indx)
				{
					case 0:
						z = atoi(St);
						break;
					case 1:
						break;
					case 2:
						break;
					case 3:
						sk = atoi(St);
						break;
					case 4:
						si = atoi(St);
						break;

				}

				++Indx;
				St = Ed + 1;
				Ed = strchr(St, ' ');

				if (Ed > rTn)
					Ed = rTn;
			}

			Pos -= sizeof(tmp) - (rTn - tmp) - 1;
			SetFilePointer(hFile, Pos, NULL, FILE_BEGIN);

			if (pvz && pvz != z)
				ZoomSeek[z - FIRST_ZOOM] = dwCntMHash;
			
			if (!_p_MHash)
				_p_MHash = (PMapHash)malloc(sizeof(MapHash));
			else
				_p_MHash = (PMapHash)realloc(_p_MHash, sizeof(MapHash) * (dwCntMHash + 1));


			_p_MHash[dwCntMHash].Seek = sk;
			_p_MHash[dwCntMHash].Size = si;

			++dwCntMHash;

			pvz = z;
		}

		CloseHandle(hFile);

		return 1;
	}

	return 0;
}

BOOL MakeBitmap(HWND hWnd, HBITMAP *_hBitmap, BYTE _Zoom, INT x, INT y, BOOL PrevRelease)
{
	HDC							hMemDC = NULL;
	HDC							hDC = NULL;
	HBITMAP						hOldBitmap;
	Gdiplus::Image				*Img = NULL;
	Gdiplus::Graphics			*graphics = NULL;
	DWORD						fIndex;
	BOOL						_SUCCESS_ERROR = FALSE;

	if (PrevRelease && *_hBitmap)
		DeleteObject(*_hBitmap);

	if (hDC = GetDC(hWnd))
	{
		if (*_hBitmap = CreateCompatibleBitmap(hDC, 256, 256))
		{
			if (hMemDC = CreateCompatibleDC(hDC))
			{
				hOldBitmap = (HBITMAP)SelectObject(hMemDC, *_hBitmap);

				if (graphics = new Gdiplus::Graphics(hMemDC))
				{
					if( (fIndex = ZoomSeek[_Zoom - FIRST_ZOOM] + (ST_MAPH + 1) * x + y) >= dwCntMHash)
						goto RELEASE;

					if (Img = ext_png(MAPPATH, _p_MHash[fIndex].Seek, _p_MHash[fIndex].Size))
						graphics->DrawImage(Img, 0, 0);
					else goto RELEASE;
				}
				else goto RELEASE;

				SelectObject(hMemDC, hOldBitmap);
			}
			else goto RELEASE;
		}
		else goto RELEASE;
	}
	else goto RELEASE;

	_SUCCESS_ERROR = TRUE;

RELEASE:

	if (!_SUCCESS_ERROR)
	{
		MessageBox(hWnd, "MakeBitmap() Function Error!!", "Error", MB_ICONERROR);
		*_hBitmap = NULL;
	}

	if(Img)
		delete(Img);
	if(graphics)
		delete(graphics);
	if(hMemDC)
		DeleteDC(hMemDC);
	if(hDC)
		ReleaseDC(hWnd, hDC);

	return _SUCCESS_ERROR;
}

VOID ShowBitmap(HDC hDC, HBITMAP *_hBitmap, long x, long y)
{
	HBITMAP						hOldBitmap;
	HDC							hMemDC;

	hMemDC = CreateCompatibleDC(hDC);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, *_hBitmap);
	BitBlt(hDC, x, y, 256, 256, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

VOID InitGDI()
{
	Gdiplus::GdiplusStartupInput         m_GdiplusStartupInput;
	ULONG_PTR							m_GdiplusToken;

	Gdiplus::GdiplusStartup(&m_GdiplusToken, &m_GdiplusStartupInput, NULL);
}

VOID InitMap(HWND hWnd, BYTE _Zoom)
{
	SetZoom(_Zoom);
	LoadSeek(HASHPATH);

	for (int j = 0; j < IMG_H; ++j)
	{
		for (int i = 0; i < IMG_W; ++i)
		{
			if (!MakeBitmap(hWnd, &_Bitmap[j][i], _Zoom, i, j, 0))
				return;
		}
	}

	LHMAX.cx = IMG_W - 1;
	LHMAX.cy = IMG_H - 1;

	InitFont(8 + (_Zoom - FIRST_ZOOM));
}

VOID InitFont(UINT Size)
{
	ReleaseFont();

	if (!MyFont)
	{
		MyFont = CreateFont(Size, // 높이
			0, // 너비
			0, // 기울기
			0, // 방향
			FW_NORMAL,	// 굵기
			0, // 기울기
			0, // 밑줄
			0, //취소선
			HANGUL_CHARSET,	// 문자셋 : DEFAULT_CHARSET , 한글 폰트를 쓰고싶으면 HANGUL_CHARSET 을 쓴다
			OUT_DEFAULT_PRECIS, // 출력정확도
			CLIP_DEFAULT_PRECIS, // 클리핑정확도
			DEFAULT_PITCH,		// 출력의 질
			0,					// 자간
			"굴림"				//폰트 이름
		);
	}
}

VOID UpdateMapRECT(HWND hWnd)
{
	GetClientRect(hWnd, &WndRect);

	WndCenter.x = WndRect.right / 2;
	WndCenter.y = WndRect.bottom / 2;

	if (ps_nsz.x < WndCenter.x) 
		ps_nsz.x = WndCenter.x;

	if (ps_nsz.y < WndCenter.y) 
		ps_nsz.y = WndCenter.y;

	if (ps_nsz.x > (ED_MAPX - ST_MAPX - 1) * 256)
	{
		ps_nsz.x = (ED_MAPX - ST_MAPX - 1) * 256;

		if (ps_nsz.x < WndRect.right)
			ps_nsz.x = WndCenter.x;
	}

	if (ps_nsz.y > (ED_MAPY - ST_MAPY - 1) * 256)
	{
		ps_nsz.y = (ED_MAPY - ST_MAPY - 1) * 256;

		if (ps_nsz.y < WndRect.bottom)
			ps_nsz.y = WndCenter.y;
	}

	MapRect.left = (ps_nsz.x - WndCenter.x) / 256;
	MapRect.top = (ps_nsz.y - WndCenter.y) / 256;

	MapRect.right = MapRect.left + (WndRect.right / 256) + 2;
	MapRect.bottom = MapRect.top + (WndRect.bottom / 256) + 2;

	if (MapRect.right > ED_MAPX - ST_MAPX)
		MapRect.right = ED_MAPX - ST_MAPX;

	if (MapRect.bottom > ED_MAPY - ST_MAPY)
		MapRect.bottom = ED_MAPY - ST_MAPY;

	__Increase.x = WndCenter.x - ps_nsz.x;
	__Increase.y = WndCenter.y - ps_nsz.y;

	__Increase.x = (-__Increase.x) % 256;
	__Increase.y = (-__Increase.y) % 256;
}


VOID DrawMap(HWND hWnd, HDC hDC, BYTE _Zoom)
{
	int							i, j, x, y, z;
	LONG						Index_X, Index_Y;
	POINT						Tile, Offset;
	HDC							hMemDC;
	HBITMAP						hBitmap, hOldBitmap;
	HPEN						hPen, hOldPen;
	char						tmp[10];

	UpdateMapRECT(hWnd);

	hBitmap = CreateCompatibleBitmap(hDC, WndRect.right, WndRect.bottom);
	hMemDC = CreateCompatibleDC(hDC);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	SelectObject(hMemDC, MyFont);
	SetBkMode(hMemDC, TRANSPARENT);

	for (j = MapRect.top; j <= MapRect.bottom; ++j)
	{
		for (i = MapRect.left; i <= MapRect.right; ++i)
		{
			x = i % IMG_W;
			y = j % IMG_H;

			if (i > LHMAX.cx || j > LHMAX.cy || i < LHMIN.cx || j < LHMIN.cy)
				if (!MakeBitmap(hWnd, &_Bitmap[y][x], _Zoom, i, j, 1))
					return;

			ShowBitmap(hMemDC, &_Bitmap[y][x], -__Increase.x + (256 * (i - MapRect.left)), -__Increase.y + (256 * (j - MapRect.top)));
		}
	}

	LHMAX.cx = MapRect.right;
	LHMAX.cy = MapRect.bottom;

	LHMIN.cx = MapRect.left;
	LHMIN.cy = MapRect.top;

	z = 0;

	for (j = 0; j < __DrawMarkupCnt; ++j)
	{
		SetLatLontoTileXY(__DrawMarkup[j].lat, __DrawMarkup[j].lon, _Zoom, &Tile, &Offset);

		Index_X = Tile.x - ST_MAPX;
		Index_Y = Tile.y - ST_MAPY;

		if (__DrawMarkup[j].Type == 2)
			z++;

		if (Index_X >= MapRect.left && Index_X <= MapRect.right && Index_Y >= MapRect.top && Index_Y <= MapRect.bottom)
		{
			hPen = CreatePen(PS_SOLID, __DrawMarkup[j].radius, __DrawMarkup[j].dwColor);
			hOldPen = (HPEN)SelectObject(hMemDC, hPen);

			Index_X = (Index_X - MapRect.left) * 256;
			Index_Y = (Index_Y - MapRect.top) * 256;

			i = _Zoom - FIRST_ZOOM;
			if (i <= 0) i = 1;

			switch (__DrawMarkup[j].Type)
			{
				case 1:
					Rectangle(hMemDC, -__Increase.x + Index_X - i + Offset.x, -__Increase.y + Index_Y - i + Offset.y,
						-__Increase.x + Index_X + i + Offset.x, -__Increase.y + Index_Y + i + Offset.y);
					break;
				case 2:
				{
					int _x, _y;

					_x = -__Increase.x + Index_X + Offset.x;
					_y = -__Increase.y + Index_Y + Offset.y;

					sprintf_s(tmp, sizeof(tmp), "%d", z);
					TextOut(hMemDC, _x - 5 - strlen(tmp), _y - i - (10 + i), tmp, strlen(tmp));

					Ellipse(hMemDC, _x - i, _y - i, _x + i, _y + i);
					break;
				}
				case 3:
				{
					LONG Index_X2, Index_Y2;
					POINT Tile2, Offset2;

					SetLatLontoTileXY(__DrawMarkup[j].lat2, __DrawMarkup[j].lon2, _Zoom, &Tile2, &Offset2);

					Index_X2 = Tile2.x - ST_MAPX;
					Index_Y2 = Tile2.y - ST_MAPY;

					if (Index_X2 >= MapRect.left && Index_X2 <= MapRect.right && Index_Y2 >= MapRect.top && Index_Y2 <= MapRect.bottom)
					{
						Index_X2 = (Index_X2 - MapRect.left) * 256;
						Index_Y2 = (Index_Y2 - MapRect.top) * 256;

						MoveToEx(hMemDC, -__Increase.x + Index_X + Offset.x,
										 -__Increase.y + Index_Y + Offset.y, NULL);

						LineTo(hMemDC,	 -__Increase.x + Index_X2 + Offset2.x,
										 -__Increase.y + Index_Y2 + Offset2.y);
					}

					break;
				}
			}

			SelectObject(hMemDC, hOldPen);
			DeleteObject(hPen);
		}
	}

	if (Mode == MODE_ROUTE && _p_PickRoute)
	{
		hPen = CreatePen(PS_SOLID, 4, 0xF324FC);
		hOldPen = (HPEN)SelectObject(hMemDC, hPen);

		for (j = 0; j < (int)(_p_PickRoute->rcnt / 2); ++j)
		{
			if (j + 1 < (int)(_p_PickRoute->rcnt / 2))
			{
				POINT Tile2, Offset2;

				SetLatLontoTileXY(_p_PickRoute->_Lat[j], _p_PickRoute->_Lng[j], Zoom, &Tile, &Offset);
				SetLatLontoTileXY(_p_PickRoute->_Lat[j + 1], _p_PickRoute->_Lng[j + 1], Zoom, &Tile2, &Offset2);

				MoveToEx(hMemDC, -__Increase.x + ((Tile.x - ST_MAPX - MapRect.left) * 256) + Offset.x,
					-__Increase.y + ((Tile.y - ST_MAPY - MapRect.top) * 256) + Offset.y, NULL);

				LineTo(hMemDC, -__Increase.x + ((Tile2.x - ST_MAPX - MapRect.left) * 256) + Offset2.x,
					-__Increase.y + ((Tile2.y - ST_MAPY - MapRect.top) * 256) + Offset2.y);
			}
		}

		SelectObject(hMemDC, hOldPen);
		DeleteObject(hPen);

	}

	BitBlt(hDC, 0, 0, WndRect.right, WndRect.bottom, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
}

VOID SetMarkUp(BYTE Type, double _lat, double _long, double _lat2, double _long2, BYTE radius, COLORREF dwColor, LPVOID pMemo)
{
	__DrawMarkup[__DrawMarkupCnt].Type = Type;
	__DrawMarkup[__DrawMarkupCnt].dwColor = dwColor;
	__DrawMarkup[__DrawMarkupCnt].lat = _lat;
	__DrawMarkup[__DrawMarkupCnt].lon = _long;
	__DrawMarkup[__DrawMarkupCnt].lat2 = _lat2;
	__DrawMarkup[__DrawMarkupCnt].lon2 = _long2;
	__DrawMarkup[__DrawMarkupCnt].radius = radius;
	__DrawMarkup[__DrawMarkupCnt].pMemo = pMemo;

	++__DrawMarkupCnt;
}

VOID DeleteMarkUp(BYTE Type, double _lat, double _long, double _lat2, double _long2)
{
	int i, j;

	for (i = 0; i < __DrawMarkupCnt; ++i)
	{
		if (__DrawMarkup[i].Type == Type &&
			__DrawMarkup[i].lat == _lat &&
			__DrawMarkup[i].lon == _long &&
			__DrawMarkup[i].lat2 == _lat2 &&
			__DrawMarkup[i].lon2 == _long2)
		{
			for (j = i; j < __DrawMarkupCnt; ++j)
			{
				__DrawMarkup[j].Type = __DrawMarkup[j + 1].Type;
				__DrawMarkup[j].dwColor = __DrawMarkup[j + 1].dwColor;
				__DrawMarkup[j].lat = __DrawMarkup[j + 1].lat;
				__DrawMarkup[j].lon = __DrawMarkup[j + 1].lon;
				__DrawMarkup[j].lat2 = __DrawMarkup[j + 1].lat2;
				__DrawMarkup[j].lon2 = __DrawMarkup[j + 1].lon2;
				__DrawMarkup[j].radius = __DrawMarkup[j + 1].radius;
				__DrawMarkup[j].pMemo = __DrawMarkup[j + 1].pMemo;
			}

			--__DrawMarkupCnt;
			break;
		}
	}
}

VOID DrawSearchRoute(DWORD lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	int i;
	int c1, c2, min_x, min_y, max_x, max_y;

	BOOL Find = FALSE;
	POINT p1, p2;

	if (!_p_Route)
		return;

	PMapROUTE next = _p_Route;

	LONG Index_X, Index_Y;
	LONG Index_X2, Index_Y2;

	while (next)
	{
		for (i = 0; i < (int)(next->rcnt / 2); ++i)
		{
			SetLatLon(next->_Lat[i], next->_Lng[i], Zoom, &p1);
			SetLatLon(next->_Lat[i + 1], next->_Lng[i + 1], Zoom, &p2);

			Index_X = p1.x / 256;
			Index_Y = p1.y / 256;
			Index_X2 = p2.x / 256;
			Index_Y2 = p2.y / 256;

			if (Index_X >= MapRect.left && Index_X <= MapRect.right && Index_Y >= MapRect.top && Index_Y <= MapRect.bottom)
			{
				if (Index_X2 >= MapRect.left && Index_X2 <= MapRect.right && Index_Y2 >= MapRect.top && Index_Y2 <= MapRect.bottom)
				{
					if (p1.x < p2.x)
					{
						max_x = p2.x;
						min_x = p1.x;
					}
					else
					{
						max_x = p1.x;
						min_x = p2.x;
					}

					if (p1.y < p2.y)
					{
						max_y = p2.y;
						min_y = p1.y;
					}
					else
					{
						max_y = p1.y;
						min_y = p2.y;
					}

					c1 = __Increase.x + MapRect.left * 256 + x;
					c2 = __Increase.y + MapRect.top * 256 + y;

					if (c1 >= min_x && c1 <= max_x && c2 >= min_y && c2 <= max_y)
					{
						_p_PickRoute = next;
						Find = TRUE;
						break;
					}
				}
			}
		}

		if (Find)
			break;

		next = next->next;
	}

	InvalidateRgn(hMapWnd, NULL, FALSE);
}

VOID ClearMarkUp()
{
	__DrawMarkupCnt = 0;
}

VOID SetClickEventFunc(CLKEVENTPROC ClickEventProc)
{
	__ClickEvent = ClickEventProc;
}

VOID SetMapRoute(PMapROUTE pRoute)
{
	_p_Route = pRoute;
}

INT IsCursorInRect(DWORD lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	int j, i;
	POINT lpp;

	for (j = 0; j < __DrawMarkupCnt; ++j)
	{
		if (__DrawMarkup[j].Type == 1 || __DrawMarkup[j].Type == 2)
		{
			SetLatLon(__DrawMarkup[j].lat, __DrawMarkup[j].lon, Zoom, &lpp);

			i = Zoom - FIRST_ZOOM;
			if (i <= 0) i = 1;

			if (__Increase.x + MapRect.left * 256 + x >= lpp.x - i && __Increase.x + MapRect.left * 256 + x <= lpp.x + i &&
				__Increase.y + MapRect.top * 256 + y >= lpp.y - i && __Increase.y + MapRect.top * 256 + y <= lpp.y + i)
				return j;
		}
	}
	
	return -1;
}

VOID PickMap(DWORD lParam, double *_Lat, double *_Long)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	int i, j;

	POINT cov;
	POINT inc;

	i = x + __Increase.x;
	j = y + __Increase.y;

	cov.x = ST_MAPX + MapRect.left + (i / 256);
	cov.y = ST_MAPY + MapRect.top + (j / 256);

	inc.x = i % 256;
	inc.y = j % 256;

	TileXYToLatLon(&cov, &inc, Zoom, _Lat, _Long);
}

DOUBLE Picked_Lat()
{
	return __DrawMarkup[0].lat;
}

DOUBLE Picked_Long()
{
	return __DrawMarkup[0].lon;
}

VOID CHANGEMAP(HWND hWnd, BYTE _Zoom)
{
	int i, j, x, y;

	SetZoom(_Zoom);
	UpdateMapRECT(hWnd);

	for (j = 0; j < IMG_H; ++j)
		for (i = 0; i < IMG_W; ++i)
			DeleteObject(_Bitmap[j][i]);

	for (j = MapRect.top; j <= MapRect.bottom; ++j)
	{
		for (i = MapRect.left; i <= MapRect.right; ++i)
		{
			x = i % IMG_W;
			y = j % IMG_H;

			if (!MakeBitmap(hWnd, &_Bitmap[y][x], _Zoom, i, j, 0))
				return;
		}
	}

	InitFont(8 + (_Zoom - FIRST_ZOOM));
}

VOID ReleaseData()
{
	int j, i;

	for (j = 0; j < IMG_H; ++j)
		for (i = 0; i < IMG_W; ++i)
			DeleteObject(_Bitmap[j][i]);

	__DrawMarkupCnt = 0;
}

VOID ReleaseSeek()
{
	if (_p_MHash)
	{
		free(_p_MHash);
		_p_MHash = NULL;
	}
}

VOID ReleaseFont()
{
	if (MyFont)
	{
		DeleteObject(MyFont);
		MyFont = NULL;
	}
}