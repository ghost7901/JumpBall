#pragma once

#define CELLX 184
#define CELLY 104

enum CellObject
{
	CO_CELL,
	CO_SIMPLEBALL,
	CO_DISPOSABLEBALL,
	CO_FORWARDBALL,
	CO_STATICBALL,
	CO_TIMERBALL,
	CO_RAZORBALLH,
	CO_RAZORBALLV,
};

struct CellInfo
{
	bool bIsCell;
	int nType;
	int nValue;
};

extern CellInfo g_cellInfo[11*12];

void CellInit();
void CellLoad(char *filename);
void CellSave(char *filename);