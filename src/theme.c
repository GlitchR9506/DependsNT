#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "importlst.h"

        
//COLORREF colorAppBG = RGB(80,130,130); // blue
//COLORREF colorAppBG = RGB(130,80,80); // red
COLORREF colorAppBG = RGB(140,80,140); // purple

COLORREF colorTopBarText = RGB(255,255,255);

COLORREF colorItemEven = RGB(255,240,255); // purple

COLORREF colorItemOdd = RGB(255,255,255);

//COLORREF colorItemDLL = RGB(100,100,100);
COLORREF colorItemDLL = RGB(140,80,140); // purple

COLORREF colorDarkText = RGB(255,255,255);

PAPPTHEME pAppTheme = NULL;

VOID ThemeUpdate(PAPPTHEME pTheme) {
        if (pTheme == NULL) {return;}
        if(pTheme->brushBackground == NULL) {
                pTheme->brushBackground = CreateSolidBrush(pTheme->colorBackground);
                if (pTheme->brushBackground == NULL) {ExitWithLastError("brush creation failure");}
        }
        if(pTheme->brushButtonInside_Active == NULL) {
                pTheme->brushButtonInside_Active = CreateSolidBrush(pTheme->colorButtonInside_Active);
        }

        if(pTheme->brushButtonInside_Inactive == NULL) {
                pTheme->brushButtonInside_Inactive = CreateSolidBrush(pTheme->colorButtonInside_Inactive);
        }
        if(pTheme->hPenHighlight == NULL) {
                pTheme->hPenHighlight = CreatePen(PS_SOLID, 1, pTheme->colorHighlight);
        }
        if(pTheme->hPenShadow == NULL) {
                pTheme->hPenShadow = CreatePen(PS_SOLID, 1, pTheme->colorShadow);
        }
        if(pTheme->hPenCheckmark == NULL) {
                pTheme->hPenCheckmark = CreatePen(PS_SOLID, 2, pTheme->crCheckmark);
        }





}

PAPPTHEME vecThemes;



BOOL GetThemeFromRegistry(LPCSTR szThemeName, DWORD dwStringLength) {
        if (szThemeName == NULL) return FALSE;

        HKEY hKey = NULL;



        LSTATUS lStatus = RegOpenKeyEx(
                HKEY_CURRENT_USER,
                DEPENDSNT_REGKEY,
                0,
                KEY_QUERY_VALUE,    
                &hKey
        );
        if (lStatus != ERROR_SUCCESS) {
                if (lStatus != ERROR_FILE_NOT_FOUND) {
                        PopupErrorWithCode(ERRORLEVEL_DEBUG, "Failed to open regsitry key", lStatus);
                }
                return FALSE;
        }
        

        
        DWORD dataSize = sizeof(CHAR) * dwStringLength;
        DWORD dataType = 0;
        lStatus = RegQueryValueEx(
                hKey,
                "currThemeName",        // Value name
                NULL,                    // Reserved (must be NULL)
                &dataType,               // Receives the actual data type
                (LPBYTE)szThemeName,          // Buffer to receive the data
                &dataSize                // In: buffer size; Out: bytes copied
        );

        RegCloseKey(hKey);

        if (lStatus != ERROR_SUCCESS) {
                if (lStatus != ERROR_FILE_NOT_FOUND) {
                        PopupErrorWithCode(ERRORLEVEL_DEBUG, "Failed to open regsitry value", lStatus);
                }
                return FALSE;
        }
        return TRUE; 

}

VOID SetThemeInitial() {
        pAppTheme = &vecThemes[0];

        CHAR szThemeName[256] = {0};
        if (!GetThemeFromRegistry(szThemeName,255)) {
                pAppTheme = &vecThemes[0];
        }

        for (int i = 0; i < vector_size(vecThemes); i++) {
                if (!strcmp(vecThemes[i].szName,szThemeName)) {
                        pAppTheme = &vecThemes[i];
                        return; 
                }
        }

}

VOID PunchMask(UINT width, UINT height, COLORREF crTransparent, HBITMAP bmSource, HBITMAP bmMask) {
        //
        // setup a HDC, and select our source bitmap into it
        //
        HDC     hdcImage = CreateCompatibleDC(NULL); 
        HBITMAP bmImageOld = SelectObject(hdcImage, bmSource);

        //
        // setup a HDC, and select out mask bitmap into it
        //
        HDC     hdcMask = CreateCompatibleDC(NULL);
        HBITMAP bmMaskOld = SelectObject(hdcMask, bmMask);


        // Set background color to `crTransparent`
        COLORREF crImageBkOld = SetBkColor(hdcImage, crTransparent);

        // Create our mask
        //    Since we are copying to a 1-bit bitmap, everything of the background color will be white,
        //    everything else will be black
        BitBlt(hdcMask,
                0,0,
                width, height,
                hdcImage,
                0,0,
                SRCCOPY
        );


        // set background color to black, set foreground color to white
        SetBkColor(hdcImage, RGB(0, 0, 0));       
        SetTextColor(hdcImage, RGB(255, 255, 255));

        // SRCAND, is boolean AND operation, writes all true(white) black pixels in the hdcMask to hdcImage
        BitBlt(hdcImage,
                0,0,
                width, height,
                hdcMask,
                0,0,
                SRCAND
        );

        //
        // Clean-up mask
        //
        SelectObject(hdcMask, bmMaskOld);
        DeleteDC(hdcMask);

        //
        // Clean-up Image
        //
        SetBkColor(hdcImage, crImageBkOld);
        SelectObject(hdcImage, bmImageOld);
        DeleteDC(hdcImage);

}

HBITMAP hbmBG = NULL;
BITMAP  hmBG = {0};

VOID InitAppThemes() {
        vector_reserve(vecThemes,6);

        //
        // Load Bitmaps
        //
        hbmBG = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));

        if (hbmBG == NULL) {
            ThrowError("Failed to load bitmap from resources!");
        }// else {
        //    GetObject(hbmBG, sizeof(BITMAP), &hmBG);
        //}


/*
        APPTHEME themeDefault = {0};
        strcpy(themeDefault.szName,"Default");
        themeDefault.colorBackground        = RGB(110,110,110);
        themeDefault.colorTopBarText        = RGB(255,255,255);
        themeDefault.colorEvenItem          = RGB(0,0,220);
        themeDefault.colorOddItem           = RGB(0,0,245);
        themeDefault.colorImportantItem     = RGB(140,140,140);
        themeDefault.colorButtonText        = RGB(0,0,0);
        themeDefault.colorImportantItemText = RGB(255,255,255);
        themeDefault.colorItemText          = RGB(255,255,255);
        themeDefault.colorItemMissing       = RGB(140,0,0);
        themeDefault.colorItemMissingText   = RGB(255,255,255);
        themeDefault.colorItemInfo          = RGB(0,0,150);
        themeDefault.colorItemInfoText      = RGB(255,255,255);
        themeDefault.colorHighlight         = RGB(0,0,0);
        themeDefault.colorShadow            = RGB(255,255,255);
        themeDefault.colorButtonInside_Active = RGB(0,100,0);
        themeDefault.colorButtonInside_Inactive = RGB(100,0,0);
        //themeDefault.brushBackground = CreatePatternBrush(hbmBG);
        //vector_push_back(vecThemes, themeDefault);*/

        APPTHEME themeSoft = {0};
        strcpy(themeSoft.szName,"Default");
        themeSoft.colorBackground        = RGB(140,140,140);
        themeSoft.colorTopBarText        = RGB(255,255,255);
        themeSoft.colorEvenItem          = RGB(240,240,255);
        themeSoft.colorOddItem           = RGB(255,255,255);
        themeSoft.colorImportantItem     = RGB(140,140,140);
        themeSoft.colorButtonText        = RGB(0,0,0);
        themeSoft.colorImportantItemText = RGB(255,255,255);
        themeSoft.colorItemText          = RGB(0,0,0);
        themeSoft.colorItemMissing       = RGB(165,97,97);
        themeSoft.colorItemMissingText   = RGB(255,255,255);
        themeSoft.colorItemInfo          = RGB(0,0,150);
        themeSoft.colorItemInfoText      = RGB(255,255,255);
        themeSoft.colorHighlight         = RGB(0,0,0);
        themeSoft.colorShadow            = RGB(255,255,255);
        themeSoft.colorButtonInside_Active   = RGB(0,100,0);
        themeSoft.colorButtonInside_Inactive = RGB(100,0,0);
        themeSoft.bmMissingItem              = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MISSINGITEMLIGHT));
        themeSoft.bmMissingItem_mask         = CreateBitmap(16,16,1,1,NULL);
        themeSoft.crCheckmark            = RGB(53,99,33);

        if (themeSoft.bmMissingItem == NULL) {
            ThrowError("Failed to load bitmap IDB_MISSINGITEMLIGHT from resources!");
        }
        PunchMask(16,16,RGB(255,000,255),themeSoft.bmMissingItem,themeSoft.bmMissingItem_mask);
        vector_push_back(vecThemes, themeSoft);


        APPTHEME themeDark = {0};
        strcpy(themeDark.szName,"Dark");
        themeDark.colorBackground        = RGB(80,80,80);
        themeDark.colorTopBarText        = RGB(0,0,0);
        themeDark.colorEvenItem          = RGB(100,100,100);
        themeDark.colorOddItem           = RGB(110,110,110);
        themeDark.colorImportantItem     = RGB(140,140,140);
        themeDark.colorButtonText        = RGB(0,0,0);
        themeDark.colorImportantItemText = RGB(0,0,0);
        themeDark.colorItemText          = RGB(0,0,0);
        themeDark.colorItemMissing       = RGB(104,42,42);
        themeDark.colorItemMissingText   = RGB(150,150,150);
        themeDark.colorItemInfo          = RGB(0,0,0);
        themeDark.colorItemInfoText      = RGB(150,150,150);
        themeDark.colorHighlight         = RGB(0,0,0);
        themeDark.colorShadow            = RGB(255,255,255);
        themeDark.colorButtonInside_Active   = RGB(0,100,0);
        themeDark.colorButtonInside_Inactive = RGB(100,0,0);
        themeDark.bmMissingItem              = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MISSINGITEMDARK));
        themeDark.bmMissingItem_mask         = CreateBitmap(16,16,1,1,NULL);
        themeDark.crCheckmark            = RGB(38,127,000);
        
        if (themeDark.bmMissingItem  == NULL) {
            ThrowError("Failed to load bitmap IDB_MISSINGITEMICON from resources!");
        }
        PunchMask(16,16,RGB(255,000,255),themeDark.bmMissingItem,themeDark.bmMissingItem_mask);
        vector_push_back(vecThemes, themeDark);
/*

        APPTHEME themePurple = {0};
        strcpy(themePurple.szName,"Purple");
        themePurple.colorBackground        = RGB(180,100,180);
        themePurple.colorTopBarText        = RGB(0,0,0);
        themePurple.colorEvenItem          = RGB(255,200,255);
        themePurple.colorOddItem           = RGB(255,240,255);
        themePurple.colorImportantItem     = RGB(140,80,140);
        themePurple.colorButtonText        = RGB(0,0,0);
        themePurple.colorImportantItemText = RGB(0,0,0);
        themePurple.colorItemText          = RGB(0,0,0);
        themePurple.colorItemMissing       = RGB(150,0,0);
        themePurple.colorItemMissingText   = RGB(255,255,255);
        themePurple.colorItemInfo          = RGB(150,0,150);
        themePurple.colorItemInfoText      = RGB(255,255,255);
        themePurple.colorHighlight         = RGB(0,0,0);
        themePurple.colorShadow            = RGB(255,255,255);
        themePurple.colorButtonInside_Active = RGB(0,100,0);
        themePurple.colorButtonInside_Inactive = RGB(100,0,0);
        //vector_push_back(vecThemes, themePurple);*/

         //Initialize stuff within them
        for (int i = 0; i < vector_size(vecThemes); i++) {
                ThemeUpdate(&vecThemes[i]);
        }



        SetThemeInitial();
}


VOID CreateFonts() {
        //
        // Create the fonts
        //
        hFont = CreateFontA(18, 0, 0, 0, 900, FALSE, FALSE, FALSE, ANSI_CHARSET, 
              OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
              DEFAULT_PITCH | FF_DONTCARE, "Courier New");

        hFontSmall = CreateFontA(16, 0, 0, 0, 900, FALSE, FALSE, FALSE, ANSI_CHARSET, 
              OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
              DEFAULT_PITCH | FF_DONTCARE, "Courier New");
}


VOID UpdateThemeMenu(HMENU hMenu) {
        while (DeleteMenu(hMenu, 0, MF_BYPOSITION)) {}
        for (int i = 0; i < vector_size(vecThemes); i++) {
                AppendMenu(hMenu, MF_STRING, IDM_THEMELIST + i, vecThemes[i].szName);
        }
}


VOID WriteThemeToRegistry(LPCSTR szName) {

        LSTATUS lStatus;
        HKEY hKey;


        lStatus = RegCreateKeyEx(
                HKEY_CURRENT_USER,     
                DEPENDSNT_REGKEY,      
                0,                     
                NULL,      
                REG_OPTION_NON_VOLATILE, 
                KEY_WRITE,              
                NULL,                   
                &hKey,                 
                NULL                     
        );
        if (lStatus != ERROR_SUCCESS) {
                PopupErrorWithCode(ERRORLEVEL_ERROR, "Failed to open registry key", lStatus);
                return;
        }

        lStatus = RegSetValueEx(
                hKey,
                "currThemeName",       
                0,                      
                REG_SZ,                
                (const BYTE *)szName, 
                (strlen(szName) + 1)*sizeof(CHAR)           
        );

        if (lStatus != ERROR_SUCCESS) {
                PopupErrorWithCode(ERRORLEVEL_ERROR, "Failed to write theme to registry", lStatus);
                RegCloseKey(hKey);
                return;
        }
}

VOID SetThemeIndex(UINT index) {
        pAppTheme = &vecThemes[index];
        WriteThemeToRegistry(pAppTheme->szName);
}