#define IDM_ORIGINAL 101
#define IDM_COPT2    102
#define IDM_TANKCOPT 103
#define IDM_ABOUT    104

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, unsigned, UINT, LONG);
BOOL FAR PASCAL About(HWND, unsigned, UINT, LONG);
