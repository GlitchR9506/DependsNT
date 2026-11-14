//
//
//
#include <windows.h>   // required for all Windows applications
#include <stdio.h>
#include "importlst.h"

char szAppName[] = "Depends31";   // The name of this application
char szTitle[]   = "Dependency Walker for NT 3.1"; // The title bar text


LRESULT CALLBACK WndProc(
                HWND hWnd,         // window handle
                UINT message,      // type of message
                WPARAM uParam,     // additional information
                LPARAM lParam);
				
/****************************************************************************

        FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPSTR, int)

        PURPOSE: calls initialization function, processes message loop

        COMMENTS:

                Windows recognizes this function by name as the initial entry point
                for the program.  This function calls the application initialization
                routine, if no other instance of the program is running, and always
                calls the instance initialization routine.  It then executes a message
                retrieval and dispatch loop that is the top-level control structure
                for the remainder of execution.  The loop is terminated when a WM_QUIT
                message is received, at which time this function exits the application
                instance by returning the value passed by PostQuitMessage().

                If this function must abort before entering the message loop, it
                returns the conventional value NULL.

****************************************************************************/


int APIENTRY WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{

        MSG msg;
        WNDCLASS  wc;

        // Fill in window class structure with parameters that describe the
        // main window.

        wc.style         = CS_HREDRAW | CS_VREDRAW;// Class style(s).
        wc.lpfnWndProc   = (WNDPROC)WndProc;       // Window Procedure
        wc.cbClsExtra    = 0;                      // No per-class extra data.
        wc.cbWndExtra    = 0;                      // No per-window extra data.
        wc.hInstance     = hInstance;              // Owner of this class
        wc.hIcon         = LoadIcon (hInstance, szAppName); // Icon name from .RC
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursor
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);// Default color
        wc.lpszMenuName  = szAppName;              // Menu name from .RC
        wc.lpszClassName = szAppName;              // Name to register as

        // Register the window class and return success/failure code.
        RegisterClass(&wc);
		
        hwndMain = CreateWindow(
                szAppName,           // See RegisterClass() call.
                szTitle,             // Text for window title bar.
                WS_OVERLAPPEDWINDOW,// Window style.
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, // Use default positioning
                NULL,                // Overlapped windows have no parent.
                NULL,                // Use the window class menu.
                hInstance,           // This instance owns this window.
                NULL                 // We don't use any data in our WM_CREATE
        );

        // If window could not be created, return "failure"
        if (!hwndMain) {
                ExitWithLastError("Failed to create Main window.");
        }

        // Make the window visible; update its client area; and return "success"
        ShowWindow(hwndMain, nCmdShow); // Show the window
        UpdateWindow(hwndMain);         // Sends WM_PAINT message
		

        /* Acquire and dispatch messages until a WM_QUIT message is received. */

        while (GetMessage(&msg, // message structure
           NULL,   // handle of window receiving the message
           0,      // lowest message to examine
           0))     // highest message to examine
        {
			TranslateMessage(&msg);// Translates virtual key codes
			DispatchMessage(&msg); // Dispatches message to window
        }


        return (msg.wParam); // Returns the value from PostQuitMessage

        lpCmdLine; // This will prevent 'unused formal parameter' warnings
}
