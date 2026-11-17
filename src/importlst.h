// importlst.h
#ifndef IMPORTLST_H
#define IMPORTLST_H
#include <windows.h>
#include <c_vector/vector.h>

#include "resource.h"

PVOID ImageRvaToVa(
    PIMAGE_NT_HEADERS NtHeaders,
    PVOID Base,
    ULONG Rva,
    PIMAGE_SECTION_HEADER *LastRvaSection);

VOID ExitWithMessage(LPCSTR szInfo);

VOID ExitWithLastError(LPCSTR szInfo) ;

VOID ThrowError(LPCSTR szInfo);

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
    BOOL bHideInUI;
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
extern HWND hwndAbout;

VOID RegisterMainWindowClass();
VOID RegisterAboutMenuClass();

extern char wndclassMain[];
extern char wndclassAbout[]; 

extern HFONT hFont;
extern HFONT hFontSmall;
VOID CreateFonts();


typedef struct {
        CHAR     szName[255];
        COLORREF colorBackground;
        COLORREF colorTopBarText;
        COLORREF colorEvenItem;
        COLORREF colorOddItem;
        COLORREF colorImportantItem;
        COLORREF colorButtonText;
        COLORREF colorImportantItemText;
        COLORREF colorItemText;
        COLORREF colorItemMissing;
        COLORREF colorItemMissingText;
        COLORREF colorItemInfo;
        COLORREF colorItemInfoText;


        COLORREF colorHighlight;
        COLORREF colorShadow;
        HPEN     hPenHighlight;
        HPEN     hPenShadow;

        COLORREF colorButtonInside_Active;
        COLORREF colorButtonInside_Inactive;
        HBRUSH   brushButtonInside_Active;
        HBRUSH   brushButtonInside_Inactive;


        HBRUSH   brushBackground;
} APPTHEME, *PAPPTHEME;

VOID InitAppThemes();
extern PAPPTHEME pAppTheme;
VOID UpdateThemeMenu(HMENU hMenu);
VOID SetThemeIndex(UINT index);

#define ERRORLEVEL_DEBUG 0 
#define ERRORLEVEL_ERROR 1
#define ERRORLEVEL_FATAL 2 


#define MINIMUM_ERROR_LEVEL ERRORLEVEL_DEBUG

typedef LONG LSTATUS;

void PopupErrorWithCode(DWORD ErrorLevel, LPCSTR szInfo, DWORD dwCode);

#define DEPENDSNT_REGKEY "Software\\GlitchR9506\\DependsNT"

//
// Missing stuff from headers that still exists on nt 3.1
//

BOOL WINAPI DrawEdge(
  [in]      HDC    hdc,
  [in, out] LPRECT qrc,
  [in]      UINT   edge,
  [in]      UINT   grfFlags
);

#define BDR_RAISEDOUTER 1
#define BDR_SUNKENOUTER 2
#define BDR_RAISEDINNER 4
#define BDR_SUNKENINNER 8
#define BDR_OUTER   3
#define BDR_INNER   0xc
#define BDR_RAISED  5
#define BDR_SUNKEN  10
#define EDGE_RAISED (BDR_RAISEDOUTER|BDR_RAISEDINNER)
#define EDGE_SUNKEN (BDR_SUNKENOUTER|BDR_SUNKENINNER)
#define EDGE_ETCHED (BDR_SUNKENOUTER|BDR_RAISEDINNER)
#define EDGE_BUMP   (BDR_RAISEDOUTER|BDR_SUNKENINNER)
#define BF_LEFT 1
#define BF_TOP  2
#define BF_RIGHT    4
#define BF_BOTTOM   8
#define BF_TOPLEFT  (BF_TOP|BF_LEFT)
#define BF_TOPRIGHT (BF_TOP|BF_RIGHT)
#define BF_BOTTOMLEFT   (BF_BOTTOM|BF_LEFT)
#define BF_BOTTOMRIGHT  (BF_BOTTOM|BF_RIGHT)
#define BF_RECT (BF_LEFT|BF_TOP|BF_RIGHT|BF_BOTTOM)
#define BF_DIAGONAL 16
#define BF_DIAGONAL_ENDTOPRIGHT (BF_DIAGONAL|BF_TOP|BF_RIGHT)
#define BF_DIAGONAL_ENDTOPLEFT  (BF_DIAGONAL|BF_TOP|BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMLEFT   (BF_DIAGONAL|BF_BOTTOM|BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMRIGHT  (BF_DIAGONAL|BF_BOTTOM|BF_RIGHT)
#define BF_MIDDLE   0x800
#define DFC_CAPTION 1
#define DFC_MENU    2
#define DFC_SCROLL  3
#define DFC_BUTTON  4
#define DFCS_CAPTIONCLOSE   0
#define DFCS_CAPTIONMIN 1
#define DFCS_CAPTIONMAX 2
#define DFCS_CAPTIONRESTORE 3
#define DFCS_CAPTIONHELP    4
#define DFCS_MENUARROW  0
#define DFCS_MENUCHECK  1
#define DFCS_MENUBULLET 2
#define DFCS_MENUARROWRIGHT 4
#define DFCS_SCROLLUP   0
#define DFCS_SCROLLDOWN 1
#define DFCS_SCROLLLEFT 2
#define DFCS_SCROLLRIGHT    3
#define DFCS_SCROLLCOMBOBOX 5
#define DFCS_SCROLLSIZEGRIP 8
#define DFCS_SCROLLSIZEGRIPRIGHT    16
#define DFCS_BUTTONCHECK    0
#define DFCS_BUTTONRADIOIMAGE   1
#define DFCS_BUTTONRADIOMASK    2
#define DFCS_BUTTONRADIO    4
#define DFCS_BUTTON3STATE   8
#define DFCS_BUTTONPUSH 16
#define DFCS_INACTIVE   256
#define DFCS_PUSHED 512
#define DFCS_CHECKED    1024





#endif