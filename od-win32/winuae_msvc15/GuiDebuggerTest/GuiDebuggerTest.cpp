// GuiDebuggerTest.cpp : Defines the entry point for the application.
//

#include <string>

#include "framework.h"
#include "GuiDebuggerTest.h"
#include "debugger/DebuggerAPI.h"
void loadDebuggerDLL(const TCHAR *dllName, TCHAR *cmdLine);
void unloadDebuggerDLL(bool bForce);

#ifdef UNICODE
typedef std::wstring TSTRING;
#else
typedef std::string TSTRING;
#endif

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];				// the main window class name

static string commandLine;

static const TCHAR defaultParams[] = _T(R"(--config c:\Users\Public\Documents\Amiga Files\WinUAE\gui_debugger_test.cfg)");

static TCHAR dbgDllPath[512];
static TCHAR dbgDllCmdLine[512];

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HACCEL hAccelTable;

static void readConfig(const char *filename);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR	lpCmdLine,
					 _In_ int	   nCmdShow)
{
	hInst = hInstance;
	wstring wcmd;
	WCHAR *p = lpCmdLine;
	while (*p)
		wcmd += *p++;

	for (int c : wcmd)
		commandLine += c;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUIDEBUGGERTEST));

	readConfig(commandLine.c_str());

	loadDebuggerDLL(&dbgDllPath[0], &dbgDllCmdLine[0]);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GUIDEBUGGERTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	unloadDebuggerDLL(true);

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style		  = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra	 = 0;
	wcex.cbWndExtra	 = 0;
	wcex.hInstance	  = hInstance;
	wcex.hIcon		  = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUIDEBUGGERTEST));
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_GUIDEBUGGERTEST);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//		In this function, we save the instance handle in a global variable and
//		create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow(szWindowClass
	   , szTitle
	   , WS_OVERLAPPEDWINDOW
	   //, CW_USEDEFAULT, 0
	   , 50, 50
	   //, CW_USEDEFAULT , 0
	   , 250 , 300
	   , nullptr
	   , nullptr
	   , hInstance
	   , nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
	
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_DEBUGGER_LOAD:
					loadDebuggerDLL(dbgDllPath, dbgDllCmdLine);
				break;

				case IDM_DEBUGGER_UNLOAD:
					unloadDebuggerDLL(false);
				break;

				case IDM_DEBUGGER_UNLOAD_FORCE:
					unloadDebuggerDLL(true);
					break;

				case IDM_DEBUGGER_HIDE:
					Debugger->ShowDebugger(SW_MINIMIZE);
				break;

				case IDM_DEBUGGER_RESTORE:
					Debugger->ShowDebugger(SW_RESTORE);
				break;

				case IDM_ABOUT:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;

				case IDM_EXIT:
					DestroyWindow(hWnd);
				break;

				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
		return (INT_PTR)TRUE;

		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK dlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_CLOSE:
			::EndDialog(hDlg, IDOK);
		return TRUE;

		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		default:
			return DefWindowProc(hDlg, Msg, wParam, lParam);
	}
	return 0;
}

static void GetDllPathFromUser(void)
{
	HMODULE hModule = ::GetModuleHandle(0);
	HINSTANCE hInst = hModule;

	HRSRC hrsrc = ::FindResource(hModule, MAKEINTRESOURCE(IDD_INPUTLINE), RT_DIALOG);

	HGLOBAL hglobal = ::LoadResource(hModule, hrsrc);

	::DialogBoxIndirectParam(hInst, (LPCDLGTEMPLATE)hglobal, 0, (DLGPROC)dlgProc, 0);

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return;
}

static void writeConfig(const char *filename)
{
#ifdef UNICODE
	static const char str[] = "%ls\n";
#else
	static const char str[] = "%s\n";
#endif

	GetDllPathFromUser();
	FILE *config = fopen(filename, "w");
	fprintf(config, str, dbgDllPath);
	fprintf(config, str, defaultParams);
	fclose(config);
}

static void readConfig(const char *filename)
{
	FILE *config = fopen(filename, "r");
	if (!config)
	{
		writeConfig(filename);
		return;
	}

	char *p = fgets(dbgDllPath, sizeof dbgDllPath, config);
	size_t i = strlen(p) - 1;
	while (i > 0 && (p[i] == '\n' || p[i] == '\r' ))
		p[i--] = 0;

	p = fgets(dbgDllCmdLine, sizeof dbgDllCmdLine, config);
	i = strlen(p) - 1;
	while (i > 0 && (p[i] == '\n' || p[i] == '\r'))
		p[i--] = 0;

	fclose(config);
}