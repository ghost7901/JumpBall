#include "cell.h"
#include <iostream>

CellInfo g_cellInfo[11*12];

void CellInit()
{
	for(int i=0;i<11;i++)
	{
		for(int n=0;n<12;n++)
		{
			g_cellInfo[i*12+n].bIsCell=false;
			g_cellInfo[i*12+n].nType=0;
			g_cellInfo[i*12+n].nValue=0;
		}
	}
}

void CellLoad(char *filename)
{
	FILE *fp=fopen(filename,"rb+");
	if(fp!=0)fread(g_cellInfo,sizeof(g_cellInfo),1,fp);
	fclose(fp);
}

void CellSave(char *filename)
{
	FILE *fp=fopen(filename,"wb+");
	if(fp!=0) fwrite(g_cellInfo,sizeof(g_cellInfo),1,fp);
	fclose(fp);
}