#define STRICT
#include <windows.h>   // required for all Windows applications
#include <stdio.h>
#include <windowsx.h>
#include "importlst.h"



HWND hwndMain;
HWND hwndListbox;
HWND hwndToggleMissingOnly;
HWND hwndTopbar;
HWND hwndBottombar;
const INT heightTopbar = 18; 
const INT heightBottombar = 18;


HMENU hMenu = NULL;
HMENU hFileMenu = NULL;
HMENU hThemeMenu = NULL;

CHAR  sz_filterExecutableFiles[] = "Executable Files\0*.EXE;*.DLL\0";
CHAR  szFilePathBuffer[MAX_PATH] = {0};

BOOL  bFileOpen = FALSE;
BOOL  bShowOnlyMissing = FALSE;

IMPORT_LIST lstImports = {0};


LRESULT CALLBACK WndProc(
                HWND hWnd,         // window handle
                UINT message,      // type of message
                WPARAM uParam,     // additional information
                LPARAM lParam);


VOID RegisterMainWindowClass() {
        WNDCLASS  wc;

        // Fill in window class structure with parameters that describe the
        // main window.

        wc.style         = CS_HREDRAW | CS_VREDRAW;// Class style(s).
        wc.lpfnWndProc   = (WNDPROC)WndProc;       // Window Procedure
        wc.cbClsExtra    = 0;                      // No per-class extra data.
        wc.cbWndExtra    = 0;                      // No per-window extra data.
        wc.hInstance     = GetModuleHandle(NULL);              // Owner of this class
        wc.hIcon         = LoadIcon (GetModuleHandle(NULL), "appicon.ico"); // Icon name from .RC
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursor
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);// Default color
        //wc.lpszMenuName  = wndclassMain;              // Menu name from .RC
        wc.lpszClassName = wndclassMain;              // Name to register as

        // Register the window class and return success/failure code.
        RegisterClass(&wc);
}


VOID RefreshImportDisplay() {
        CHAR szBuffer[256] = {0};

        if (bFileOpen == FALSE) return;


        SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0); 

        //SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)szFilePathBuffer);
        SetWindowText(hwndTopbar, szFilePathBuffer);
        SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)"NF[ ] == OK; [M] == MISSING");


        for (int i = 0; i < lstImports.nImportedDLLs;i++) {
                BOOL bFoundDLL = lstImports.arrImportedDLLs[i].bFound;
                sprintf(szBuffer, "D%c%s %s",bFoundDLL ? 'F' : 'M',lstImports.arrImportedDLLs[i].szName, bFoundDLL ? "" : "MISSING");
                SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)szBuffer);

                if (lstImports.arrImportedDLLs[i].bHideInUI) continue;
                for (int j = 0; j < lstImports.arrImportedDLLs[i].nImports; j++) {
                        BOOL bFoundImport = lstImports.arrImportedDLLs[i].arrImports[j].bFound;
                        if (bShowOnlyMissing == FALSE || bFoundImport == FALSE) {
                                if (lstImports.arrImportedDLLs[i].arrImports[j].szName[0] == '\0') {
                                        sprintf(szBuffer, "I%c ORDINAL[%#lX/%lu]",bFoundImport ? ' ' : 'M', lstImports.arrImportedDLLs[i].arrImports[j].Ordinal,lstImports.arrImportedDLLs[i].arrImports[j].Ordinal);
                                } else {
                                        sprintf(szBuffer, "I%c %s",bFoundImport ? ' ' : 'M',lstImports.arrImportedDLLs[i].arrImports[j].szName);
                                }
                                SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)szBuffer);
                        }
                }
        }
}

VOID ParseFileAndDisplay(LPCSTR szFilePath) {
        DestroyImportList(&lstImports);
        SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0); 

        CHAR szBuffer[256] = {0};
        
        // Open file and map to memory
        PVOID pTargetFileBaseAddress = OpenTargetFileAndMapToMemory(szFilePath);
        
        //printf("File opened successfully and is valid, examining header for imports... \n");
        
        // Read thru the imports of the PE
        if (ReadImports(pTargetFileBaseAddress, &lstImports) == -1) {
                CloseTargetedFile();
                //ThrowError("File is invalid\r\n");
                return;
        }
        if (lstImports.arrImportedDLLs == NULL) {
                CloseTargetedFile();
                ThrowError("internal error\r\n");
                return;
        }
        // Close the file and related handles
        CloseTargetedFile();

        bFileOpen = TRUE;

        RefreshImportDisplay();


}
/****************************************************************************

        FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)

        PURPOSE:  Processes messages

        MESSAGES:

        WM_COMMAND    - application menu (About dialog box)
        WM_DESTROY    - destroy window

        COMMENTS:

        To process the IDM_ABOUT message, call MakeProcInstance() to get the
        current instance address of the About() function.  Then call Dialog
        box which will create the box according to the information in your
        generic.rc file and turn control over to the About() function.  When
        it returns, free the intance address.

****************************************************************************/




//ULONG lastClick = 0;
//WNDPROC g_OrigListBoxProc = NULL;
//LRESULT CALLBACK ListBoxSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    if (msg == WM_ERASEBKGND)
//    {
        //HDC hdc = (HDC)wParam;
        //RECT rc;
        //GetClientRect(hwnd, &rc);
        //FillRect(hdc, &rc, pAppTheme->brushBackground); // Pass g_hEmptyBgBrush as dwRefData
        //return 1; // Tell Windows the background was erased
 //   }       
 //   if (msg == WM_PAINT)
 //   {
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hwnd, &ps);
            //FillRect(hdc, &ps.rcPaint, pAppTheme->brushBackground);
            //EndPaint(hwnd, &ps);
            //return 0;
//    }
    
//    return CallWindowProc(g_OrigListBoxProc, hwnd, msg, wParam, lParam);
//}


VOID DNT_DrawEdge(HDC hdc, PRECT pRect, HBRUSH brushBackground, HPEN hPenTopLeft, HPEN hPenBottomRight) {
        if (pRect == NULL) {return;};

        HGDIOBJ oldPen = SelectObject(hdc, hPenTopLeft);

        if (brushBackground != NULL) {
                FillRect(hdc, pRect,brushBackground);
        }

        MoveToEx(hdc, pRect->left,pRect->bottom -1, NULL);
        LineTo(hdc,pRect->left,pRect->top);
        LineTo(hdc,pRect->right -1, pRect->top);


        SelectObject(hdc, hPenBottomRight);
        MoveToEx(hdc,pRect->right -1, pRect->top, NULL);
        LineTo(hdc,pRect->right-1,pRect->bottom-1);
        LineTo(hdc,pRect->left-1,pRect->bottom-1);

        SelectObject(hdc, oldPen);
}

VOID DNT_DrawButton(HDC hdc, PRECT pRect, BOOL bActive) {
        HPEN hPenTopLeft     = bActive ? pAppTheme->hPenHighlight : pAppTheme->hPenShadow;
        HPEN hPenBottomRight = bActive ? pAppTheme->hPenShadow : pAppTheme->hPenHighlight;
        HBRUSH hBrushBackground = bActive ? pAppTheme->brushButtonInside_Active : pAppTheme->brushButtonInside_Inactive;

        DNT_DrawEdge(hdc, pRect, hBrushBackground, hPenTopLeft, hPenBottomRight);
        InflateRect(pRect, -1, -1);
        DNT_DrawEdge(hdc, pRect, hBrushBackground, hPenTopLeft, hPenBottomRight);
}

VOID DNT_DrawCheckBox(HDC hdc, PRECT pRect, BOOL bActive) {
        HPEN hPenTopLeft     = pAppTheme->hPenHighlight;
        HPEN hPenBottomRight =  pAppTheme->hPenShadow;
        HBRUSH hBrushBackground = pAppTheme->brushBackground;

        DNT_DrawEdge(hdc, pRect, hBrushBackground, hPenTopLeft, hPenBottomRight);
        InflateRect(pRect, -1, -1);
        DNT_DrawEdge(hdc, pRect, hBrushBackground, hPenTopLeft, hPenBottomRight);

        if (bActive) {
                HGDIOBJ oldPen = SelectObject(hdc, pAppTheme->hPenCheckmark);

                UINT width  = pRect->right  - pRect->left;
                UINT height = pRect->bottom - pRect->top;

                MoveToEx(hdc, pRect->left + (width * 20)/100,pRect->top + (height * 50)/100, NULL);
                LineTo(hdc,pRect->left + (width * 40)/100,pRect->top + (height * 80)/100);
                LineTo(hdc,pRect->left + (width * 80)/100,pRect->top + (height * 15)/100);
                //LineTo(hdc,pRect->right -1, pRect->top);

                SelectObject(hdc, oldPen);
        }

}


VOID DNT_DrawDropdownIcon(HDC hdc, PRECT pRect, BOOL bActive) {


        //HPEN hPenTopLeft     = bActive ? pAppTheme->hPenShadow : pAppTheme->hPenHighlight;
        //HPEN hPenBottomRight = bActive ? pAppTheme->hPenHighlight : pAppTheme->hPenShadow;
        //HBRUSH hBrushBackground = bActive ? pAppTheme->brushButtonInside_Active : pAppTheme->brushButtonInside_Inactive;


        POINT ptActive[3] = {
                {pRect->left+(pRect->right - pRect->left)/2, pRect->bottom},
                {pRect->left, pRect->top},
                {pRect->right, pRect->top}
        };
        POINT ptInactive[3] = {
                {pRect->left, pRect->top},
                {pRect->left, pRect->bottom},
                {pRect->right, pRect->top + (pRect->bottom - pRect->top)/2}
        };

        HGDIOBJ oldPen = SelectObject(hdc, bActive ? pAppTheme->hPenShadow : pAppTheme->hPenHighlight);
        HGDIOBJ oldBrush = SelectObject(hdc, bActive ? pAppTheme->brushButtonInside_Active : pAppTheme->brushButtonInside_Inactive);

        Polygon(hdc, bActive ? ptActive : ptInactive, 3); // Draws and fills the triangle

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);




        //DNT_DrawEdge(hdc, pRect, hBrushBackground, hPenTopLeft, hPenBottomRight);
        //InflateRect(pRect, -1, -1);
        //DNT_DrawEdge(hdc, pRect, hBrushBackground, hPenTopLeft, hPenBottomRight);
}



LRESULT CALLBACK WndProc(
                HWND hWnd,         // window handle
                UINT message,      // type of message
                WPARAM wParam,     // additional information
                LPARAM lParam)     // additional information
{
	RECT rect;
	BOOL bSuccess;
        OPENFILENAME fileName = {0};

        switch (message) {

                //case WM_MESSUREITEM:
                        //LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
                        //switch (lpmis->CtrID) {
                        //        case IDC_IMPORTLIST:
                        //                break;
                        //}
                        //break;
                case WM_CTLCOLORSTATIC: {
                        HDC hdcStatic = (HDC)wParam;
                        switch(GetDlgCtrlID((HWND)lParam)) {
                                case IDC_TOPBAR: {

                                        HDC hdc = (HDC)wParam;
                                        SetTextColor(hdc, pAppTheme->colorTopBarText); 
                                        SetBkMode(hdc, TRANSPARENT);
                                        return (LRESULT)pAppTheme->brushBackground;
                                        break;}
                                case IDC_BOTTOMBAR: {
                                        HDC hdc = (HDC)wParam;
                                        SetTextColor(hdc, pAppTheme->colorTopBarText); 
                                        SetBkMode(hdc, TRANSPARENT);
                                        return (LRESULT)pAppTheme->brushBackground;
                                        break;}
                        }
                        break;
                }/*
                case WM_CTLCOLORBTN:
                        switch(GetDlgCtrlID((HWND)lParam)) {
                                case IDC_TOGGLE_ONLYMISSING:

                                        HDC hdc = (HDC)wParam;
                                        SetTextColor(hdc, RGB(255, 255, 255));
                                        SetBkMode(hdc, TRANSPARENT);
                                        return (LRESULT)brushBG;
                                        break;
                        } */
                case WM_CTLCOLORLISTBOX: {
                        HDC hdcListBox = (HDC)wParam;
                        HWND hwndListBox = (HWND)lParam;
                        switch (GetDlgCtrlID(hwndListBox)) {
                                case IDC_IMPORTLIST: {
                                        SetBkColor(hdcListBox, pAppTheme->colorBackground);
                                        return (LRESULT)pAppTheme->brushBackground;
                                }
                        break;
                        }
                }
                case WM_DRAWITEM:
                        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
                        switch (lpdis->CtlID) {
                                case IDC_IMPORTLIST: {

                                        COLORREF colorBG =   lpdis->itemID % 2 ? pAppTheme->colorEvenItem : pAppTheme->colorOddItem;
                                        COLORREF colorText =  pAppTheme->colorItemText;
                                        CHAR szBuffer[256+2] = {0};
                                        SendMessage(lpdis->hwndItem,LB_GETTEXT,lpdis->itemID,(LPARAM)szBuffer);

                                        BOOL bIsDLL = FALSE;//strchr(szBuffer, '.') != NULL;
                                        BOOL bIsMissing = FALSE;

                                        // [0]D = DLL, N = INFO, I = IMPORT;[1] M = MISSING, F = FOUND...

                                        if (szBuffer[0] == 'D') {
                                                bIsDLL = TRUE;
                                                colorBG = pAppTheme->colorImportantItem;
                                                colorText = pAppTheme->colorImportantItemText;
                                        } else if (szBuffer[0] == 'I') {
                                                //colorBG = pAppTheme->colorItemMissing;
                                                //colorText = pAppTheme->colorItemMissingText;
                                                if (szBuffer[1] == 'M') {
                                                        bIsMissing = TRUE;
                                                        colorBG = pAppTheme->colorItemMissing;
                                                        colorText = pAppTheme->colorItemMissingText;
                                                }
                                        } else if (szBuffer[0] == 'N') {
                                                colorBG = pAppTheme->colorItemInfo;
                                                colorText = pAppTheme->colorItemInfoText;
                                        }

                                        // advance past info for print
                                        LPCSTR szPrint = &szBuffer[2];


                                        HBRUSH hBrush = CreateSolidBrush(colorBG);
                                        FillRect(lpdis->hDC, &lpdis->rcItem,hBrush);
                                        DeleteObject(hBrush);

                                        if (bIsMissing) {
                                                UINT width = 16; UINT height = 16;
                                                HDC hdcMem = CreateCompatibleDC(lpdis->hDC);


                                                // Set background to white, foreground to black
                                                COLORREF crOldBk = SetBkColor(lpdis->hDC, RGB(255, 255, 255));
                                                COLORREF crOldText = SetTextColor(lpdis->hDC, RGB(0, 0, 0));

                                                // Save default bm to select back later, where the last selected bm wont be locked
                                                HBITMAP bmOld = SelectObject(hdcMem, pAppTheme->bmMissingItem_mask);

                                                // clear spot on screen where the image is not blended against existing
                                                BitBlt(lpdis->hDC,
                                                        lpdis->rcItem.left, lpdis->rcItem.top,
                                                        width,height,
                                                        hdcMem,
                                                        0,0,
                                                        SRCAND
                                                );

                                                // write from out bitmap
                                                SelectObject(hdcMem, pAppTheme->bmMissingItem);
                                                BitBlt(lpdis->hDC,
                                                        lpdis->rcItem.left, lpdis->rcItem.top,
                                                        width,height,
                                                        hdcMem,
                                                        0,0,
                                                        SRCPAINT
                                                );

                                                // set colors back for the owner
                                                SetBkColor(lpdis->hDC,crOldBk);
                                                SetTextColor(lpdis->hDC,crOldText);

                                                // deselect out bm and delete the device context
                                                SelectObject(hdcMem,bmOld);
                                                DeleteDC(hdcMem);







                                        }

                                        if (bIsDLL) {
                                                BOOL bIsHidden;
                                                RECT rcIcon = lpdis->rcItem;
                                                for (int i = 0; i < lstImports.nImportedDLLs;i++) {
                                                        if (strstr(szPrint,lstImports.arrImportedDLLs[i].szName) != NULL) {
                                                                bIsHidden = lstImports.arrImportedDLLs[i].bHideInUI;
                                                                break;
                                                        }
                                                }
                                                rcIcon.left += 3;
                                                rcIcon.right = rcIcon.left + 10;
                                                rcIcon.top += 3;
                                                rcIcon.bottom = rcIcon.top + 10;
                                                DNT_DrawDropdownIcon(lpdis->hDC, &rcIcon, !bIsHidden);
                                        }


                                        SetBkMode(lpdis->hDC, TRANSPARENT);
                                        SetTextColor(lpdis->hDC, colorText);

                                        RECT rcText = lpdis->rcItem;
                                        rcText.left += 16;
                                        DrawText(lpdis->hDC, szPrint, -1, &rcText, DT_SINGLELINE | DT_VCENTER | DT_LEFT);



                                        break;}
                                case IDC_TOGGLE_ONLYMISSING: {

                                        CHAR szBuffer[256] = {0};
                                        GetWindowText(lpdis->hwndItem,szBuffer,255);

                                        FillRect(lpdis->hDC, &lpdis->rcItem,pAppTheme->brushBackground);

                                        SetBkMode(lpdis->hDC, TRANSPARENT);
                                        SetTextColor(lpdis->hDC, pAppTheme->colorButtonText);

                                        RECT rcText = lpdis->rcItem;
                                        rcText.left += 36;
                                        DrawText(lpdis->hDC, szBuffer, -1, &rcText, DT_SINGLELINE | DT_BOTTOM | DT_LEFT);


                                        RECT rcCheckbox = lpdis->rcItem;
                                        rcCheckbox.left += 6;
                                        rcCheckbox.top += (lpdis->rcItem.bottom-24)/2+1;
                                        rcCheckbox.right = 24+6;
                                        rcCheckbox.bottom = 24;

                                        //FrameRect(lpdis->hDC, &rcCheckbox,bShowOnlyMissing ? pAppTheme->colorOddItem : pAppTheme->colorEvenItem);


                                        DNT_DrawCheckBox(lpdis->hDC, &rcCheckbox, bShowOnlyMissing);

                                        //UINT edge = (lpdis->itemState & ODS_SELECTED) ? EDGE_SUNKEN : EDGE_RAISED;
                                        //DrawEdge(lpdis->hDC, &lpdis->rcItem, edge, BF_RECT);

                                        break;}
                        }
                        break;

                case WM_COMMAND:  // message: command from application menu
                        if (LOWORD(wParam) >= IDM_THEMELIST && LOWORD(wParam) <= IDM_THEMLISTEND) {
                                SetThemeIndex(LOWORD(wParam) - IDM_THEMELIST);
                                InvalidateRect(hWnd, NULL, TRUE);
                                UpdateWindow(hWnd);
                                break;
                        }
                        // LOWORD(wParam) contains the ID of the clicked menu item
                        switch (LOWORD(wParam)) {

                                case IDM_FILE_OPEN:
                                        fileName.lStructSize = sizeof(fileName);
                                        fileName.hwndOwner = hWnd;
                                        fileName.hInstance = NULL;
                                        fileName.lpstrFilter = sz_filterExecutableFiles;
                                        fileName.lpstrCustomFilter = NULL;
                                        fileName.nMaxCustFilter = 0;
                                        fileName.nFilterIndex = 1;
                                        fileName.lpstrFile = szFilePathBuffer;
                                        fileName.nMaxFile = MAX_PATH;
                                        fileName.lpstrFileTitle = NULL;
                                        fileName.nMaxFileTitle = 0;
                                        // todo: save last directory
                                        fileName.lpstrInitialDir = NULL;
                                        fileName.lpstrTitle = "Open";
                                        fileName.Flags = OFN_FILEMUSTEXIST;
                                        fileName.nFileOffset;
                                        fileName.nFileExtension;
                                        fileName.lpstrDefExt = NULL;
                                        fileName.lCustData;
                                        fileName.lpfnHook = NULL;
                                        fileName.lpTemplateName;

                                        // todo examine return reason
                                        if (GetOpenFileName(&fileName) == FALSE) { 
                                                DWORD dwError = CommDlgExtendedError();
                                                switch(dwError) {
                                                        case CDERR_FINDRESFAILURE:
                                                                MessageBox(hWnd, "CDERR_FINDRESFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_INITIALIZATION:
                                                                MessageBox(hWnd, "CDERR_INITIALIZATION", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_LOCKRESFAILURE:
                                                                MessageBox(hWnd, "CDERR_LOCKRESFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_LOADRESFAILURE:
                                                                MessageBox(hWnd, "CDERR_LOADRESFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_LOADSTRFAILURE:
                                                                MessageBox(hWnd, "CDERR_LOADSTRFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_MEMALLOCFAILURE:
                                                                MessageBox(hWnd, "CDERR_MEMALLOCFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_MEMLOCKFAILURE:
                                                                MessageBox(hWnd, "CDERR_MEMLOCKFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_NOHINSTANCE:
                                                                MessageBox(hWnd, "CDERR_NOHINSTANCE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_NOHOOK:
                                                                MessageBox(hWnd, "CDERR_NOHOOK", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_NOTEMPLATE:
                                                                MessageBox(hWnd, "CDERR_NOTEMPLATE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case CDERR_STRUCTSIZE:
                                                                MessageBox(hWnd, "CDERR_STRUCTSIZE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case FNERR_BUFFERTOOSMALL:
                                                                MessageBox(hWnd, "FNERR_BUFFERTOOSMALL", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case FNERR_INVALIDFILENAME:
                                                                MessageBox(hWnd, "FNERR_INVALIDFILENAME", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                        case FNERR_SUBCLASSFAILURE:
                                                                MessageBox(hWnd, "FNERR_SUBCLASSFAILURE", "Info", MB_OK | MB_ICONINFORMATION);
                                                                break;
                                                }
                                                break;
                                        }

                                        // Set current directory to file's directory
                                        CHAR szPath[MAX_PATH];
                                        strcpy(szPath,szFilePathBuffer);
                                        szPath[fileName.nFileOffset] = '\0';
                                        SetCurrentDirectory(szPath);

                                        // todo: Send a message...
                                        ParseFileAndDisplay(szFilePathBuffer);

                                        break;

                                case IDM_FILE_EXIT:
                                        DestroyWindow(hWnd);
                                        break;
                                case IDM_ABOUT:
                                        ShowWindow(hwndAbout, SW_SHOW);
                                        break;
                                case IDC_IMPORTLIST:
                                        if (HIWORD(wParam) ==  LBN_SELCHANGE) {
                                                HWND hImportList = (HWND)lParam;
                                                UINT index = (UINT)SendMessage(hImportList, LB_GETCURSEL, 0, 0);


                                                CHAR szBuffer[256] = {0};
                                                SendMessage(hImportList,LB_GETTEXT,index,(LPARAM)szBuffer);



                                                if (strchr(szBuffer, '.') == NULL) { break;}

                                                for (int i = 0; i < lstImports.nImportedDLLs;i++) {
                                                        if (strstr(szBuffer,lstImports.arrImportedDLLs[i].szName) != NULL) {
                                                                lstImports.arrImportedDLLs[i].bHideInUI = !lstImports.arrImportedDLLs[i].bHideInUI;
                                                                RefreshImportDisplay();
                                                                break;
                                                        }
                                                }
                                        }
                                        break;
                                case IDC_TOGGLE_ONLYMISSING:
                                        if (HIWORD(wParam) == BN_CLICKED) {
                                                //ULONG now = GetTickCount();
                                                //if (now - lastClick < 50) break;
                                                bShowOnlyMissing = !bShowOnlyMissing;//IsDlgButtonChecked(hWnd, IDC_TOGGLE_ONLYMISSING);
                                                //SendMessage(hwndToggleMissingOnly, BM_SETSTATE, (UINT)bShowOnlyMissing, 0);
                                                InvalidateRect(hwndToggleMissingOnly, NULL, TRUE);
                                                UpdateWindow(hwndToggleMissingOnly);
                                                //ThrowError("aa");
                                                //lastClick = now;
                                        }

                                        RefreshImportDisplay();

                                        break;
                        }
			break;
		case WM_CREATE :
                        //=========================================================================================
                        //
                        //
                        // MISC. EXTERNAL FUNCTIONS
                        //
                        //
                        //=========================================================================================

                        //
                        // Init Themes
                        //
                        InitAppThemes();

                        //
                        // Create fonts
                        //
                        CreateFonts();

                        //=========================================================================================
                        //
                        //
                        // RESOURCES
                        //
                        //
                        //=========================================================================================


                        //=========================================================================================
                        //
                        //
                        // MENUS
                        //
                        //
                        //=========================================================================================


                        //
                        // Create the menu bar
                        //
                        hMenu      = CreateMenu();
                        hFileMenu  = CreatePopupMenu();
                        hThemeMenu = CreatePopupMenu();

                        if (hMenu == NULL) { ExitWithLastError("Failed to create menu.");}
                        if (hFileMenu == NULL) { ExitWithLastError("Failed to create file popup menu.");}
                        if (hThemeMenu == NULL) { ExitWithLastError("Failed to create theme menu.");}

                        //
                        // hFileMenu
                        //

                        AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "&Open");
                        AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, "E&xit");

                        //
                        // hThemeMenu
                        //

                        UpdateThemeMenu(hThemeMenu);

                        //
                        // Append the submenus to the menu bar
                        //
                        if (!AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu,"&File")) ExitWithLastError("Menu creation failed, AppendMenu File.");
                        if (!AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hThemeMenu,"&Themes")) ExitWithLastError("Menu creation failed, AppendMenu Themes.");
                        if (!AppendMenu(hMenu, MF_STRING, IDM_ABOUT,"&About"))         ExitWithLastError("Menu creation failed, AppendMenu About.");


                        //
                        // Set the menu as this window's menu
                        //

                        if (!SetMenu(hWnd,hMenu)) {
                                ExitWithLastError("Failed to attach menu to window.");
                        }

                        //=========================================================================================
                        //
                        //
                        // CHILD WINDOWS
                        //
                        //
                        //=========================================================================================

                        // Create the list
                        GetClientRect (hWnd, &rect);
                        hwndListbox = CreateWindowA(
                                "LISTBOX",
                                "",
                                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY  | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
                                0,
                                heightTopbar,
                                rect.right,
                                max(rect.bottom - 100, 0),
                                hWnd,
                                (HMENU)IDC_IMPORTLIST,
                                GetModuleHandle(NULL),
                                NULL
                        );
                        if (hwndListbox == NULL) ExitWithLastError("Failed to create LISTBOX.");

                        UpdateWindow(hwndListbox);

                        // Check check button
                        GetClientRect (hWnd, &rect);
                        hwndToggleMissingOnly = CreateWindowA(
                                "BUTTON",
                                "Only Show Missing",
                                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                                0,
                                max(rect.bottom - 100, 0),
                                rect.right,
                                25,
                                hWnd,
                                (HMENU)IDC_TOGGLE_ONLYMISSING,
                                GetModuleHandle(NULL),
                                NULL
                        );
                        if (hwndToggleMissingOnly == NULL) ExitWithLastError("Failed to create BUTTON.");

//g_OrigListBoxProc = SubclassWindow(hwndToggleMissingOnly, (LONG)ListBoxSubclassProc);
                        UpdateWindow(hwndToggleMissingOnly);

                        //=========================================================================================
                        //
                        //
                        // STATIC WINDOWS
                        //
                        //
                        //=========================================================================================

                        GetClientRect (hWnd, &rect);
                        hwndTopbar = CreateWindowA(
                                "STATIC",
                                "No File Open...",
                                WS_CHILD | WS_VISIBLE | SS_LEFT,
                                0,
                                0,
                                rect.right,
                                heightTopbar,
                                hWnd,
                                (HMENU)IDC_TOPBAR,
                                GetModuleHandle(NULL),
                                NULL
                        );
                        if (hwndTopbar == NULL) ExitWithLastError("Failed to create BUTTON.");

                        GetClientRect (hWnd, &rect);
                        hwndBottombar = CreateWindowA(
                                "STATIC",
                                "Dependency Walker for Windows NT 3.1+, v1.1, 2026, GlitchR9506",
                                WS_CHILD | WS_VISIBLE | SS_LEFT,
                                0,
                                rect.bottom - heightBottombar,
                                rect.right,
                                heightBottombar,
                                hWnd,
                                (HMENU)IDC_BOTTOMBAR,
                                GetModuleHandle(NULL),
                                NULL
                        );
                        if (hwndBottombar == NULL) ExitWithLastError("Failed to create BUTTON.");


                        //=========================================================================================
                        //
                        //
                        // POPUP WINDOWS
                        //
                        //
                        //=========================================================================================

                        hwndAbout = CreateWindowA(
                                wndclassAbout,
                                "About DependsNT",
                                WS_POPUP | WS_CAPTION,
                                 100,
                                 100,
                                 300,
                                 300,
                                hWnd,
                                NULL,
                                GetModuleHandle(NULL),
                                NULL
                        );
                        if (hwndAbout == NULL) ExitWithLastError("Failed to create About Menu.");


                        //
                        // set font
                        //
                        SendMessage(hwndListbox, WM_SETFONT, (UINT)hFont, TRUE);
                        SendMessage(hwndToggleMissingOnly, WM_SETFONT, (UINT)hFont, TRUE);
                        SendMessage(hwndAbout, WM_SETFONT, (UINT)hFont, TRUE);
                        SendMessage(hwndTopbar, WM_SETFONT, (UINT)hFont, TRUE);
                        SendMessage(hwndBottombar, WM_SETFONT, (UINT)hFont, TRUE);


			break;



                case WM_SIZE:
                        GetClientRect (hWnd, &rect);
                        SetWindowPos(
                            hwndListbox, 
                            NULL, 
                            0, 0,           // Ignored due to SWP_NOMOVE
                            rect.right, max(rect.bottom - 100-heightTopbar, 0),        // New width and height
                            SWP_NOMOVE | SWP_NOZORDER
                        );
                        SetWindowPos(
                            hwndToggleMissingOnly, 
                            NULL, 
                                 0,
                                max(rect.bottom - 100, 0),
                                rect.right,
                                25,
                             SWP_NOZORDER
                        );
                        SetWindowPos(
                                hwndBottombar,
                                NULL, 
                                0,
                                rect.bottom-heightBottombar,
                                rect.right,
                                heightBottombar,
                             SWP_NOZORDER
                        );
                        UpdateWindow(hwndToggleMissingOnly);
                        break;
                case WM_PAINT :

					HDC hdc;
					PAINTSTRUCT ps;
					hdc = BeginPaint (hWnd, &ps);
					GetClientRect (hWnd, &rect);
					FillRect(hdc,&rect,pAppTheme->brushBackground);
					rect.bottom = 30;
					DrawText (hdc, szFilePathBuffer, -1, &rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					EndPaint (hWnd, &ps); 
					break;
                case WM_DESTROY:  // message: window being destroyed
                        PostQuitMessage(0);
                        break;

                default:          // Passes it on if unproccessed
                        return (DefWindowProc(hWnd, message, wParam, lParam));
        }
        return (0);
}
