#define DEBUGGER_CALLBACK_PROC_IMPL

#include <thread>

#include "gui/AmiPalIDE.h"
#include "gui/MainFrame.h"

#include "debugger/DebuggerAPI.h"

using namespace std;

static const wchar_t *gAppName = L"AmiPalIDE";

wxIMPLEMENT_APP(AmiPalApp);

static bool gDebuggerExited = false;
AmiPalApp *gApp = nullptr;
static thread *gGuiThread = nullptr;
static wchar_t ErrorText[265];

static vector<wstring> parseCmdLine(wchar_t *cmdLine)
{
	int argc = 0;

	WCHAR **argv = ::CommandLineToArgvW(cmdLine, &argc);

	vector<wstring> args;

	args.push_back(gAppName);
	for (int i = 0; i < argc; i++)
		args.push_back(argv[i]);

	if (argc)
		::LocalFree(argv);

	return args;
}

static void runDebugger(wchar_t *cmdLine)
{
	vector<wstring> args = parseCmdLine(cmdLine);
	vector<WCHAR *> argv;

	for (wstring &s : args)
		argv.push_back(&s[0]);
	argv.push_back(nullptr);

	int argc = (int)argv.size();
	wxEntry(argc, &argv[0]);

	gApp = nullptr;
	gDebuggerExited = true;
}

static inline MainFrame *getAppFrame(void)
{
	if (gDebuggerExited)
		return nullptr;

	return MainFrame::getInstance();
}

static bool isCompatible(uint32_t clientVersion)
{
	if (clientVersion <= getVersion())
		return true;

	return false;
}

extern "C" WINUAE_DEBUGGER_API wchar_t *InitDebugger(uint32_t version, wchar_t *cmdLine)
{
	if (!isCompatible(version))
	{
		_snwprintf(ErrorText, sizeof(ErrorText), L"Client API version %08lX is incompatible with debugger version %08lX", version, getVersion());

		return ErrorText;
	}

	if (!gGuiThread)
	{
		if (gDebuggerExited)
		{
			gGuiThread->join();
			delete gGuiThread;
			gApp = nullptr;
			gGuiThread = nullptr;
		}
	}

	gDebuggerExited = false;
	gGuiThread = new thread(runDebugger, cmdLine);

	// Wait until the debugger window has initialized.
	volatile AmiPalApp **app = (volatile AmiPalApp **)&gApp;
	while (!(*app))
		Sleep(100);

	return nullptr;
}

extern "C" WINUAE_DEBUGGER_API void CloseDebugger(void)
{
	if (!gDebuggerExited)
	{
		MainFrame *frame = MainFrame::getInstance();
		if (frame)
			frame->Close(true);
	}

	if (gGuiThread)
	{
		gGuiThread->join();
		delete gGuiThread;
		gApp = nullptr;
		gGuiThread = nullptr;
	}
}

extern "C" WINUAE_DEBUGGER_API void ShowDebugger(int cmdShow)
{
	MainFrame *frame = getAppFrame();
	if (!frame)
		return;

	switch (cmdShow)
	{
		case SW_RESTORE:
			frame->Show();
		break;

		case SW_MINIMIZE:
		break;

		case SW_MAXIMIZE:
		break;

		case SW_HIDE:
		break;
	}
}
