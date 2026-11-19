#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "importlst.h"

void ExitWithMessage(LPCSTR szInfo) {
	
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten = 0;
	
	WriteConsoleA(hConsole, szInfo, lstrlenA(szInfo), &charsWritten, NULL);
    MessageBoxA(NULL, szInfo, "EXITING", MB_OK | MB_ICONSTOP);
	
	
	TerminateProcess(GetCurrentProcess(), (UINT)0);
}
void ExitWithLastError(LPCSTR szInfo) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten = 0;
	DWORD dwLastError = GetLastError();
	CHAR szBuffer[256] = {0};
	
	if (szInfo == NULL) {
		sprintf(szBuffer, "EXIT with code [%lx]\r\n", dwLastError);
	} else {
		sprintf(szBuffer, "EXITING, CODE [%lx]: %s\r\n", dwLastError, szInfo);
	}
	
	WriteConsoleA(hConsole, szBuffer, lstrlenA(szBuffer), &charsWritten, NULL);

    MessageBoxA(NULL, szBuffer, "EXITING", MB_OK | MB_ICONSTOP);
	
	TerminateProcess(GetCurrentProcess(), (UINT)dwLastError);
}



void PopupErrorWithCode(DWORD ErrorLevel, LPCSTR szInfo, DWORD dwCode) {
	if (ErrorLevel < MINIMUM_ERROR_LEVEL) {return;}

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charsWritten = 0;
	CHAR szBuffer[256] = {0};
	
	if (szInfo == NULL) {
		sprintf(szBuffer, "EXIT with code [%lx]\r\n", dwCode);
	} else {
		sprintf(szBuffer, "EXITING, CODE [%lx]: %s\r\n", dwCode, szInfo);
	}
	
	WriteConsoleA(hConsole, szBuffer, lstrlenA(szBuffer), &charsWritten, NULL);

    MessageBoxA(NULL, szBuffer, "ERROR", MB_OK | MB_ICONSTOP);
	
	if (ErrorLevel == ERRORLEVEL_FATAL)
		TerminateProcess(GetCurrentProcess(), (UINT)dwCode);
}

// change into a message box later if make gui application
VOID ThrowError(LPCSTR szInfo) {
	//printf(szInfo);
    MessageBoxA(NULL, szInfo, "ERROR", MB_OK | MB_ICONSTOP);
	return;
}