#include <windows.h>   // required for all Windows applications
#include <stdio.h>
#include "importlst.h"

HWND hwndAbout = NULL;
HWND hWndCloseButton = NULL;

LRESULT CALLBACK procAbout(
                HWND hWnd,         // window handle
                UINT message,      // type of message
                WPARAM wParam,     // additional information
                LPARAM lParam)     // additional information
;


VOID RegisterAboutMenuClass() {
        WNDCLASS  wc;

        // Fill in window class structure with parameters that describe the
        // main window.

        wc.style         = CS_HREDRAW | CS_VREDRAW;// Class style(s).
        wc.lpfnWndProc   = (WNDPROC)procAbout;       // Window Procedure
        wc.cbClsExtra    = 0;                      // No per-class extra data.
        wc.cbWndExtra    = 0;                      // No per-window extra data.
        wc.hInstance     = GetModuleHandle(NULL);              // Owner of this class
        wc.hIcon         = LoadIcon (GetModuleHandle(NULL), "appicon.ico"); // Icon name from .RC
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursor
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);// Default color
        //wc.lpszMenuName  = szAppName;              // Menu name from .RC
        wc.lpszClassName = wndclassAbout;              // Name to register as

        // Register the window class and return success/failure code.
        RegisterClass(&wc);
}

HBITMAP bmpMiku = NULL;
BITMAP  bmMiku = {0};

LRESULT CALLBACK procAbout(
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

                case IDC_ABOUT_CLOSE:
                    ShowWindow(hWnd, SW_HIDE);
                    break;
            }
			break;

		case WM_CREATE :
            GetClientRect (hWnd, &rect);

            // Check check button
            hWndCloseButton = CreateWindowA(
                    "BUTTON",
                    "Close",
                    WS_CHILD | WS_VISIBLE,
                    rect.right-100-5,
                    rect.bottom-25-5,
                    100,
                    25,
                    hWnd,
                    (HMENU)IDC_ABOUT_CLOSE,
                    GetModuleHandle(NULL),
                    NULL
            );
            if (hWndCloseButton == NULL) ExitWithLastError("Failed to create BUTTON.");
            SendMessage(hWndCloseButton, WM_SETFONT, (UINT)hFont, TRUE);

            bmpMiku = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MIKU));

            if (bmpMiku == NULL) {
                ThrowError("Failed to load bitmap from resources!");
            } else {
                GetObject(bmpMiku, sizeof(BITMAP), &bmMiku);
            }

			break;



        case WM_SIZE:
            GetClientRect (hWnd, &rect);
            break;
        case WM_PAINT :
            RECT rcText = {0};
            GetClientRect (hWnd, &rect);
            GetClientRect (hWnd, &rcText);

			hdc = BeginPaint (hWnd, &ps);
			HFONT oldFont = SelectObject(hdc, hFontSmall);
            DrawText (hdc, "DependsNT", -1, &rcText,DT_SINGLELINE | DT_CENTER);
            rcText.top+=20;rcText.bottom-=40;
            DrawText (hdc, "Dependency Walker for Windows NT3.1 and above.\r\nBy GlitchR9506", -1, &rcText,DT_LEFT | DT_WORDBREAK);
			SelectObject(hdc, oldFont);

            if (bmpMiku) {
                HDC hdcMem = CreateCompatibleDC(hdc); 
                HBITMAP oldBitmap = SelectObject(hdcMem, bmpMiku);
                BitBlt(hdc,0,rect.bottom-bmMiku.bmHeight,bmMiku.bmWidth,bmMiku.bmHeight, hdcMem,0,0,SRCCOPY);
                SelectObject(hdcMem, oldBitmap);
                DeleteDC(hdcMem);
            }

			EndPaint (hWnd, &ps); 
			break;
        case WM_DESTROY:  // message: window being destroyed
            //PostQuitMessage(0);
            break;

        default:          // Passes it on if unproccessed
                return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}
