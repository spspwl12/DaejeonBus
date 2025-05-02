#ifndef VIRTUALBUS_H
#define VIRTUALBUS_H

#include "dbfunc.h"
#include <time.h>

void ADDVBS(BUSSTOP *BusStop, BUS *Bus, UINT Num, UINT Type)
{
	UINT cnt;

	if (!(cnt = BusStop->__vbscnt))
		BusStop->__vbs = (VBS*)malloc(sizeof(VBS));
	else
		BusStop->__vbs = (VBS*)realloc(BusStop->__vbs, sizeof(VBS) * (cnt + 1));

	BusStop->__vbs[cnt].BusCarNum = Num;
	BusStop->__vbs[cnt].BusCarType = Type;
	BusStop->__vbs[cnt].StopBus = Bus;

	++BusStop->__vbscnt;
}

void DELVBS(BUSSTOP *BusStop, BUS *Bus)
{
	UINT cnt, i, j;

	if (!(cnt = BusStop->__vbscnt))
		return;

	if (cnt == 1)
	{
		free(BusStop->__vbs);
		BusStop->__vbs = NULL;
		BusStop->__vbscnt = 0;

		return;
	}

	for (i = 0; i < cnt; ++i)
	{
		if (BusStop->__vbs[i].StopBus == Bus)
		{
			for (j = i; j < cnt - 1; ++j)
				BusStop->__vbs[j] = BusStop->__vbs[j + 1];

			break;
		}
	}

	BusStop->__vbs = (VBS*)realloc(BusStop->__vbs, sizeof(VBS) * (cnt - 1));
	--BusStop->__vbscnt;
}

bool MOVEVBS(BUSSTOP *dst_BusStop, BUSSTOP *src_BusStop, BUS *Bus)
{
	UINT i, s_cnt, d_cnt;

	if (!(s_cnt = src_BusStop->__vbscnt))
		return 0;

	if (d_cnt = dst_BusStop->__vbscnt)
	{
		for (i = 0; i < d_cnt; ++i)
		{
			if (dst_BusStop->__vbs[i].StopBus == Bus)
				return 0;
		}
	}


	for (i = 0; i < s_cnt; ++i)
	{
		if (src_BusStop->__vbs[i].StopBus == Bus)
		{
			ADDVBS(dst_BusStop, Bus, src_BusStop->__vbs[i].BusCarNum, src_BusStop->__vbs[i].BusCarType);
			DELVBS(src_BusStop, Bus);

			return 1;
		}
	}

	return 0;
}

bool availableVBS(BUSSTOP *BusStop, BUS *Bus)
{
	UINT cnt, i;

	if (!(cnt = BusStop->__vbscnt))
		return 0;

	for (i = 0; i < cnt; ++i)
	{
		if (BusStop->__vbs[i].StopBus == Bus)
			return 1;
	}

	return 0;
}

void MakeBus()
{
	BUS *next = _BUSST;
	UINT g;

	srand((unsigned int)time(NULL));

	while (next)
	{
		if ((g = next->cntBstop) && next->p_BusStop)
		{
			for (UINT i = 0; i < g; ++i)
			{
				if (!(rand() % 10))
					ADDVBS(next->p_BusStop[i], next, (rand() % 9000) + 1000, (rand() % 2) + 2);
			}
		}

		next = next->next;
	}
}

void MoveBus()
{
	BUS *next = _BUSST;
	UINT g;

	srand((unsigned int)time(NULL));

	while (next)
	{
		if ((g = next->cntBstop) && next->p_BusStop)
		{
			if (availableVBS(next->p_BusStop[g - 1], next))
			{
				DELVBS(next->p_BusStop[g - 1], next);
			}
			else
			{
				for (UINT i = 0; i < g - 1; i++)
				{
					if (rand() % 2)
					{
						if (MOVEVBS(next->p_BusStop[i + 1], next->p_BusStop[i], next))
							++i;
					}
				}
			}
		}

		next = next->next;
	}
}

int CalcBusTime(BUS *Bus, BUSSTOP *BusStop, UINT pos)
{
	UINT j;
	int i;
	VBS *vbs;

	if (Bus && BusStop && (j = Bus->cntBstop) > pos && Bus->p_BusStop)
	{
		for (i = pos; i >= 0; --i)
		{
			vbs = Bus->p_BusStop[i]->__vbs;

			for (j = 0; j < Bus->p_BusStop[i]->__vbscnt; ++j)
			{
				if (vbs[j].StopBus && vbs[j].StopBus == Bus)
					return (pos - i);
			}
		}
	}

	return -1;
}

#endif