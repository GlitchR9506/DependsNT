// resource.h

#define IDI_APPICON         101
#define IDB_MISSINGITEMLIGHT 102
#define IDB_MISSINGITEMDARK 103

#define IDB_MIKU       104
#define IDB_BACKGROUND 105
#define IDB_GIF        106

#define IDM_FILE_OPEN  1001
#define IDM_FILE_EXIT  1002
#define IDM_ABOUT      1100
#define IDC_TOGGLE_ONLYMISSING 2000
#define IDC_IMPORTLIST         2001
#define IDC_TOPBAR             2002
#define IDC_BOTTOMBAR          2003
#define IDC_ABOUT_CLOSE        3000
#define IDM_THEME              4000
#define IDM_THEMELIST          4100
#define IDM_THEMLISTEND        4200

#define ID_IDEMPTY             5000

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1

#define STR_HELPER(X) #X
#define STR(X) STR_HELPER(X)

#define VERSION_STRING STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH)