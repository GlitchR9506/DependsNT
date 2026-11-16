#include <windows.h>

// im sorry...

typedef void (__cdecl *_PVFV)(void);
void _initterm(_PVFV *pfbegin, _PVFV *pfend);
void exit(int status);

// Linker sections for C and C++ global/static initializers
#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIZ", long, read)
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)

__declspec(allocate(".CRT$XIA")) _PVFV __xi_a[] = { 0 };
__declspec(allocate(".CRT$XIZ")) _PVFV __xi_z[] = { 0 };
__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = { 0 };
__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = { 0 };

// Forward-declare your WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

void WinMainCRTStartup(void) {

    // 2. Run C and C++ global initializers
    _initterm(__xi_a, __xi_z);
    _initterm(__xc_a, __xc_z);

    // 3. Parse command line to strip the executable name (matching standard WinMain behavior)
    LPSTR lpCmdLine = GetCommandLineA();
    if (*lpCmdLine == '\"') {
        lpCmdLine++;
        while (*lpCmdLine && *lpCmdLine != '\"') lpCmdLine++;
        if (*lpCmdLine == '\"') lpCmdLine++;
    } else {
        while (*lpCmdLine && *lpCmdLine != ' ' && *lpCmdLine != '\t') lpCmdLine++;
    }
    while (*lpCmdLine == ' ' || *lpCmdLine == '\t') lpCmdLine++;

    // 4. Get STARTUPINFO for nCmdShow
    STARTUPINFOA si;
    si.dwFlags = 0;
    GetStartupInfoA(&si);
    int nCmdShow = (si.dwFlags & STARTF_USESHOWWINDOW) ? si.wShowWindow : SW_SHOWDEFAULT;

    // 5. Call WinMain and exit cleanly through crtdll
    int ret = WinMain(GetModuleHandleA(NULL), NULL, lpCmdLine, nCmdShow);

    exit(ret);
    ExitProcess(ret);
}