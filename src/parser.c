#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "importlst.h"

	
VOID DestroyImportList(PIMPORT_LIST pImportList) {
	if (pImportList == NULL) return; // no error here

    for (int i = 0; i < pImportList->nImportedDLLs;i++) {

            vector_free(pImportList->arrImportedDLLs[i].arrImports);
    }
    vector_free(pImportList->arrImportedDLLs);

    pImportList->nImportedDLLs = 0;
    pImportList->arrImportedDLLs = NULL;

}

LONG ReadImports(PVOID pBaseAddress, PIMPORT_LIST pImportList) {
	if (pImportList == NULL) {
		ThrowError("INTERNAL, pImportList NULL\r\n");
		return -1;
	}
	if (pImportList->nImportedDLLs != 0) {
		DestroyImportList(pImportList);
	}

	PIMAGE_DOS_HEADER dosHeaderAddress = (PIMAGE_DOS_HEADER)pBaseAddress;
	if (dosHeaderAddress->e_magic != IMAGE_DOS_SIGNATURE) {
		ThrowError("DOS SIGNATURE not found in file header\r\n");
		return -1;
	}
	PIMAGE_NT_HEADERS ntHeaderAddress  = (PIMAGE_NT_HEADERS)((PBYTE)pBaseAddress + dosHeaderAddress->e_lfanew);
	if (ntHeaderAddress->Signature != IMAGE_NT_SIGNATURE) {
		ThrowError("File is not a PE file (or is corrupt)!!!\r\n");
		return -1;
	}
	
	//
	// RVA == Relative Virtual Address
	//
	
	// todo: error checking if optional header exists and is valid, uhh for now, YOLO
	DWORD importDirRVA = ntHeaderAddress->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	
	// todo: technically not an error...
	if (importDirRVA == 0) {
		ThrowError("File has no imports listed\r\n");
		return -1;
	}
	PIMAGE_IMPORT_DESCRIPTOR importDesc_ = 
		(PIMAGE_IMPORT_DESCRIPTOR)ImageRvaToVa(
			ntHeaderAddress,
			pBaseAddress, 
			importDirRVA,
			NULL
			);
	PIMAGE_IMPORT_DESCRIPTOR importDesc = importDesc_;

	pImportList->arrImportedDLLs = vector_init(IMPORTED_DLL_INFO, 2);

	while (importDesc != 0 && importDesc->Name != 0) {
		LPCSTR dllName = (LPCSTR)ImageRvaToVa(ntHeaderAddress, pBaseAddress, importDesc->Name, NULL);
		IMPORTED_DLL_INFO dllInfo = {0};
		strcpy(dllInfo.szName, dllName);
		vector_push_back(pImportList->arrImportedDLLs, dllInfo);

		pImportList->arrImportedDLLs[pImportList->nImportedDLLs].arrImports = vector_init(IMPORT_INFO, 2);

		PIMAGE_THUNK_DATA pThunkData = (PIMAGE_THUNK_DATA)ImageRvaToVa(ntHeaderAddress,pBaseAddress, (ULONG)importDesc->FirstThunk, NULL);
		
		HINSTANCE hDll = LoadLibraryA(dllName);
		pImportList->arrImportedDLLs[pImportList->nImportedDLLs].bFound = hDll == NULL ? FALSE : TRUE;

		while (pThunkData != 0 && pThunkData->u1.AddressOfData != 0) {
			IMPORT_INFO importInfo = {0};

			if (IMAGE_SNAP_BY_ORDINAL(pThunkData->u1.Ordinal)) {
				strcpy(importInfo.szName, "\0");
				importInfo.bFound = TRUE;
				importInfo.Ordinal = pThunkData->u1.Ordinal;
			} else {
				PIMAGE_IMPORT_BY_NAME pImportNameData = (PIMAGE_IMPORT_BY_NAME)ImageRvaToVa(ntHeaderAddress,pBaseAddress, (ULONG)pThunkData->u1.AddressOfData, NULL);
				LPCSTR szImportName = pImportNameData->Name;
				strcpy(importInfo.szName,pImportNameData->Name);

				importInfo.bFound = FALSE;
				if (hDll != NULL) {
					FARPROC pImport = GetProcAddress(hDll, szImportName);
					if (pImport != NULL) {
						importInfo.bFound = TRUE;
					}
				} 
			}
			vector_push_back(pImportList->arrImportedDLLs[pImportList->nImportedDLLs].arrImports, importInfo);
			pImportList->arrImportedDLLs[pImportList->nImportedDLLs].nImports++;
			pThunkData++;
		}
		if (hDll != NULL) FreeLibrary(hDll);
		pImportList->nImportedDLLs++;
		importDesc++;
	}
	
	return 0;
}
