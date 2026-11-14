// importlst.h
#ifndef IMPORTLST_H
#define IMPORTLST_H
#include <c_vector/vector.h>

PVOID ImageRvaToVa(
    PIMAGE_NT_HEADERS NtHeaders,
    PVOID Base,
    ULONG Rva,
    PIMAGE_SECTION_HEADER *LastRvaSection);

VOID ExitWithMessage(LPCSTR szInfo);

VOID ExitWithLastError(LPCSTR szInfo) ;

VOID ThrowError(LPCSTR szInfo);

#define ID_FILE_OPEN  1001
#define ID_FILE_EXIT  1002
#define IDC_TOGGLE_ONLYMISSING 2000

typedef UINT UINT_PTR;

#define MAX_IMPORT_NAME_LEN 256
typedef struct {
    BOOL bFound;
    CHAR  szName[MAX_IMPORT_NAME_LEN];
    ULONG Ordinal;
} IMPORT_INFO, *PIMPORT_INFO;

typedef struct {
    CHAR szName[MAX_PATH];
    ULONG nImports;
    PIMPORT_INFO arrImports;
    BOOL bFound;
} IMPORTED_DLL_INFO, *PIMPORTED_DLL_INFO;

typedef struct {
    INT nImportedDLLs;
    PIMPORTED_DLL_INFO arrImportedDLLs;
} IMPORT_LIST, *PIMPORT_LIST;

VOID DestroyImportList(PIMPORT_LIST pImportList);


PVOID OpenTargetFileAndMapToMemory(LPCSTR szTargetPath);
VOID CloseTargetedFile();
LONG ReadImports(PVOID pBaseAddress, PIMPORT_LIST pImportList) ;


extern HWND hwndMain;



#endif