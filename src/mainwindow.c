#include <windows.h>   // required for all Windows applications
#include <stdio.h>
#include "importlst.h"



HWND hwndMain;
HWND hwndListbox;
HWND hwndToggleMissingOnly;

HMENU hMenu = NULL;
HMENU hFileMenu = NULL;

CHAR  sz_filterExecutableFiles[] = "Executable Files\0*.EXE;*.DLL\0";
CHAR  szFilePathBuffer[MAX_PATH] = {0};

BOOL  bFileOpen = FALSE;
BOOL  bShowOnlyMissing = FALSE;

IMPORT_LIST lstImports = {0};


VOID RefreshImportDisplay() {
        CHAR szBuffer[256] = {0};

        if (bFileOpen == FALSE) return;


        SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0); 

        SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)szFilePathBuffer);
        SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)"[ ] == OK; [M] == MISSING");


        for (int i = 0; i < lstImports.nImportedDLLs;i++) {
                BOOL bFoundDLL = lstImports.arrImportedDLLs[i].bFound;
                sprintf(szBuffer, "%s\t%s",lstImports.arrImportedDLLs[i].szName, bFoundDLL ? "" : "MISSING");
                SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)szBuffer);

                for (int j = 0; j < lstImports.arrImportedDLLs[i].nImports; j++) {
                        BOOL bFoundImport = lstImports.arrImportedDLLs[i].arrImports[j].bFound;
                        if (bShowOnlyMissing == FALSE || bFoundImport == FALSE) {
                                if (lstImports.arrImportedDLLs[i].arrImports[j].szName[0] == '\0') {
                                        sprintf(szBuffer, "[%c] ORDINAL[%u]",bFoundImport ? ' ' : 'M', lstImports.arrImportedDLLs[i].arrImports[j].Ordinal);
                                } else {
                                        sprintf(szBuffer, "[%c] \t%s",bFoundImport ? ' ' : 'M',lstImports.arrImportedDLLs[i].arrImports[j].szName);
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
                ExitWithMessage("File is invalid\r\n");
        }
        if (lstImports.arrImportedDLLs == NULL) {
                CloseTargetedFile();
                ExitWithMessage("internal error\r\n");
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

LRESULT CALLBACK WndProc(
                HWND hWnd,         // window handle
                UINT message,      // type of message
                WPARAM wParam,     // additional information
                LPARAM lParam)     // additional information
{
        HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	BOOL bSuccess;
        OPENFILENAME fileName = {0};

        switch (message) {

                case WM_COMMAND:  // message: command from application menu

                        // LOWORD(wParam) contains the ID of the clicked menu item
                        switch (LOWORD(wParam)) {
                                case ID_FILE_OPEN:
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

                                case ID_FILE_EXIT:
                                        DestroyWindow(hWnd);
                                        break;
                                case IDC_TOGGLE_ONLYMISSING:
                                        if (HIWORD(wParam) == BN_CLICKED) {
                                                bShowOnlyMissing = IsDlgButtonChecked(hWnd, IDC_TOGGLE_ONLYMISSING);
                                        }
                                        RefreshImportDisplay();
                                        break;
                        }
			break;
		case WM_CREATE :
                        //
                        // Create the font
                        //
                        HFONT hFont = CreateFontA(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                              OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                              DEFAULT_PITCH | FF_DONTCARE, "Courier New");

                        //
                        // Create the menu bar
                        //
                        hMenu = CreateMenu();
                        if (hMenu == NULL) { ExitWithLastError("Failed to create menu.");}

                        // File popup menu
                        hFileMenu = CreatePopupMenu();
                        if (hFileMenu == NULL) { ExitWithLastError("Failed to create file popup menu.");}
                        AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, "&Open");
                        AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, "E&xit");

                        // Append Entries to main menu
			bSuccess = AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu,"&File");
			if (bSuccess == FALSE) ExitWithLastError("Menu creation failed, AppendMenu File.");

                        // Set the menu
                        if (!SetMenu(hWnd,hMenu)) 
                                ExitWithLastError("Failed to attach menu to window.");

                        // Create the list
                        GetClientRect (hWnd, &rect);
                        hwndListbox = CreateWindow(
                                "LISTBOX",
                                "",
                                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS ,
                                0,
                                0,
                                rect.right,
                                max(rect.bottom - 100, 0),
                                hWnd,
                                NULL,
                                GetModuleHandle(NULL),
                                NULL
                        );
                        if (hwndListbox == NULL) ExitWithLastError("Failed to create LISTBOX.");

                        UpdateWindow(hwndListbox);

                        // Check check button
                        GetClientRect (hWnd, &rect);
                        hwndToggleMissingOnly = CreateWindow(
                                "BUTTON",
                                "Only Show Missing",
                                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
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

                        UpdateWindow(hwndToggleMissingOnly);

                        //
                        // set font
                        //
                        SendMessage(hwndListbox, WM_SETFONT, (UINT)hFont, TRUE);
                        SendMessage(hwndToggleMissingOnly, WM_SETFONT, (UINT)hFont, TRUE);

			break;



                case WM_SIZE:
                        GetClientRect (hWnd, &rect);
                        SetWindowPos(
                            hwndListbox, 
                            NULL, 
                            0, 0,           // Ignored due to SWP_NOMOVE
                            rect.right, max(rect.bottom - 100, 0),        // New width and height
                            SWP_NOMOVE | SWP_NOZORDER
                        );
                        break;
                case WM_PAINT :
			hdc = BeginPaint (hWnd, &ps) ;
			GetClientRect (hWnd, &rect) ;
			DrawText (hdc, "Hello, Windows NT 3.1!", -1, &rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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
