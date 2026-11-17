//
//
//
#include <windows.h>   // required for all Windows applications
#include <stdio.h>
#include "importlst.h"

HFONT hFont = NULL;
HFONT hFontSmall = NULL;

char szTitle[]   = "Dependency Walker for NT 3.1"; // The title bar text
char wndclassMain[] = "DependsNT";   // The name of this application
char wndclassAbout[] = "DependsNT_About";
				
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

        RegisterMainWindowClass();
        RegisterAboutMenuClass();
		
        hwndMain = CreateWindow(
                wndclassMain,           // See RegisterClass() call.
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

        CreateFonts();

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