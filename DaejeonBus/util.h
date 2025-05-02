#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>
#include <stdarg.h>

VOID ShowGroupsWindow(HWND DialogHandle, UINT MIN_CTRLID, UINT MAX_CTRLID, INT nCmdShow)
{
	for (UINT i = MIN_CTRLID; i <= MAX_CTRLID; ++i)
		ShowWindow(GetDlgItem(DialogHandle, i), nCmdShow);
}

VOID ComboBox_AddString(HWND hCombo, INT nMaxCounts, LPSTR *lpAddStr)
{
	int i;

	for (i = 0; i < nMaxCounts; ++i)
		SendMessage(hCombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)(*(lpAddStr + i)));
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
#endif