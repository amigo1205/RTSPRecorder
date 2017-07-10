// RTSPRecorder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string.h>
#include <process.h>
#include "ffmpeg.h"

char ** pharse(char * str, int * num);
void RecordByTime(char * address, int sec);
void RecordBySize(char * address, int size);
char ExeFileInfo[1024] = { 0 };

char * SetRecordPath(char* szRecordPath, char* szCamera)
{
	char szPath[1024];
	memset(szPath, 0, 1024);

	char* szPos;
	if (strlen(szRecordPath) == 0)
	{
		GetModuleFileNameA(NULL, ExeFileInfo, 1024);
		szPos = strrchr(ExeFileInfo, '\\');
		if (szPos != NULL)
			*(szPos + 1) = 0;

	}
	else
	{
		strcpy(ExeFileInfo, szRecordPath);
		szPos = (char*)(ExeFileInfo + strlen(ExeFileInfo));
	}
	strcat(szPos, "\\");
	CreateDirectoryA(ExeFileInfo, NULL);
	strcat(szPos, szCamera);
	strcat(szPos, "\\");

	CreateDirectoryA(ExeFileInfo, NULL);

	//create directory by current date.
	char szTime[32];
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf(szTime, "%02d%02d%02d", time.wYear, time.wMonth, time.wDay);
	strcat(szPos, szTime);
	strcat(szPos, "\\");
	CreateDirectoryA(ExeFileInfo, NULL);
	return ExeFileInfo;
}

int MyThread(DWORD *param)
{
	int nArgc = (int)param[0];
	char** szArgv = (char**)param[1];
	StartRecord(nArgc, szArgv);
	return 0;
}
__declspec(dllexport) char* __stdcall RecordRtspStreamByTime(char* szRTSPURL, char* szRecordPath, char* szCamera, int nInterval)
{
	GetModuleFileNameA(NULL, ExeFileInfo, 1024);
	char ** InputParam = (char**)malloc(sizeof(char*) * 17);
	InputParam[0] = strrchr(ExeFileInfo, '\\') + 1;
	InputParam[1] = "-i";
	InputParam[2] = szRTSPURL;
	InputParam[3] = "-an";
	InputParam[4] = "-c";
	InputParam[5] = "copy";
	InputParam[6] = "-map";
	InputParam[7] = "0";
	InputParam[8] = "-f";
	InputParam[9] = "segment";
	InputParam[10] = "-segment_time";
	int nTime = nInterval * 60;
	char tmpbuff[128] = { 0 };
	sprintf(tmpbuff, "%d", nTime);
	InputParam[11] = tmpbuff;
	InputParam[12] = "-segment_format";
	InputParam[13] = "mp4";
	InputParam[14] = "-strftime";
	InputParam[15] = "1";
	InputParam[16] = SetRecordPath(szRecordPath, szCamera);
	char ret[1024] = { 0 };
	strcpy(ret, InputParam[16], strlen(InputParam[16]));
	strcat(InputParam[16], "%H%M%S.mp4");
	DWORD dwParam[2];
	dwParam[0] = 17;
	dwParam[1] = (DWORD)InputParam;
	_beginthreadex(NULL, 0, (void*)MyThread, dwParam, 0, NULL);
	return ret;
}

__declspec(dllexport) void __stdcall Close()
{
	exit_program(1);
}

