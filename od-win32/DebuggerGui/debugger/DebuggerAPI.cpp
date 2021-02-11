#include "stdafx.h"

#define DEBUGGER_CALLBACK_PROC_IMPL
#include "DebuggerAPI.h"
#include "app/DebuggerGui.h"
#include "app/MainFrame.h"

static bool InitDebuggerAPI(void);

static bool DebuggerAPIInitialized = InitDebuggerAPI();

static bool InitDebuggerAPI(void)
{
	return true;
}

extern "C" WINUAE_DEBUGGER_API bool InitDebugger(bool firstTime)
{
	return DebuggerGuiApp::getInstance().InitDebugger(firstTime);
}

extern "C" WINUAE_DEBUGGER_API void CloseDebugger(void)
{
	DebuggerGuiApp::getInstance().CloseDebugger();
}

extern "C" WINUAE_DEBUGGER_API void ShowDebugger(int cmdShow)
{
	DebuggerGuiApp::getInstance().m_mainFrame->ShowWindow(cmdShow);
}
