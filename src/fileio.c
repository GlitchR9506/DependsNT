#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "importlst.h"

	
// Make GCC happy, it auto-injects a call to __main at the start of any function named 'main'
// we are using the c runtime libraries from visual c++, so initialization is different and this function is unneeded.
//int __main(int argc, char *argv[]) {}

HANDLE hTargetFile = NULL;
HANDLE hTargetMappingObject = NULL;
PVOID  hTargetMappedAddress = NULL;

PVOID OpenTargetFileAndMapToMemory(LPCSTR szTargetPath) {
	DWORD  dwTargetFileSizeHigh = 0;
	DWORD  dwTargetFileSizeLow = 0;
	
	// Attempt to open file of specified path.
	hTargetFile = CreateFileA(
		szTargetPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hTargetFile == INVALID_HANDLE_VALUE) {
		ExitWithLastError("Opening File failed.");
	}
	
	// check file size is over 0, error if not.
	dwTargetFileSizeLow = GetFileSize(hTargetFile, &dwTargetFileSizeHigh);
	if (dwTargetFileSizeLow == 0xFFFFFFFF) {
		CloseHandle(hTargetFile);
		ExitWithLastError("GetFileSize failed.");
	}
	// File must be larger than 0 bytes
	if (dwTargetFileSizeLow == 0 && dwTargetFileSizeHigh == 0) {
		CloseHandle(hTargetFile);
		ExitWithMessage("File must be larger than 0 bytes\r\n");
	}
	
	
	// Create a file mapping, we will then actually map the file next
	hTargetMappingObject = CreateFileMappingA(
		hTargetFile,
		NULL,
		PAGE_READONLY,
		0, // high, max size, 0 = file size
		0, // low, max size, 0 = file size
		NULL // idk what to name it, Bill or George, or anything but Sue...
	); 
	if (hTargetMappingObject == NULL) {
		CloseHandle(hTargetFile);
		ExitWithLastError("CreateFileMappingA Failed.");
	}
	
	// map file into memory so we can examine its imports
	hTargetMappedAddress = MapViewOfFile(
		hTargetMappingObject,
		FILE_MAP_READ,
		0, // file offset high
		0, // file offset low
		0  // 0 = the entire file
	);
	if (hTargetMappedAddress == NULL) {
		CloseHandle(hTargetMappingObject);
		CloseHandle(hTargetFile);
		ExitWithLastError("MapViewOfFile Failed.");
	}
	
	return hTargetMappedAddress;
}

VOID CloseTargetedFile() {
	UnmapViewOfFile(hTargetMappedAddress);
	CloseHandle(hTargetMappingObject);
	CloseHandle(hTargetFile);
	return;
}

