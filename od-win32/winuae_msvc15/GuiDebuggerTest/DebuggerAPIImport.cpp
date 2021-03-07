#include <string>
#include <iostream>

#include <Windows.h>

#define DEBUGGER_DLL_PROC_IMPL
#include "debugger/DebuggerAPI.h"

void loadDebuggerDLL(const TCHAR *dllName, TCHAR *cmdLine);
void unloadDebuggerDLL(bool bForce);
static void initAPI(void);

using namespace std;

namespace
{
	HMODULE gDebuggerDll = NULL;
	bool gDebuggerExited = true;
	IDebugger dbg;

	// This is just a helper, so the DLL will be unloaded when the application exits
	// and we don't need to modify the application code, to call unloading at the right time.
	class Unloader
	{
	public:
		Unloader() {}
		~Unloader()
		{
			unloadDebuggerDLL(true);
		}
	};
	Unloader uld;
}
IDebugger *Debugger = &dbg;

void loadDebuggerDLL(const TCHAR *dllName, TCHAR *cmdLine)
{
	if (gDebuggerDll)
	{
		// If the debugger has exited by the user, we need to unload the DLL before
		// we can restart it.
		if (gDebuggerExited)
			unloadDebuggerDLL(false);
		else
		{
			// The debugger was still running, so we can just restore it.
			Debugger->ShowDebugger(SW_RESTORE);
			return;
		}
	}

	gDebuggerDll = LoadLibrary(dllName);
	if (!gDebuggerDll)
		return;

	initAPI();

#ifdef _UNICODE
	wstring cmd(cmdLine);
#else
	wstring cmd;
	char *p = cmdLine;
	while (*p)
		cmd += *p++;
#endif

	gDebuggerExited = true;
	wchar_t *error;
	if ((error = InitDebugger(getVersion(), &cmd[0], Debugger)) != nullptr)
	{
		MessageBoxW(NULL, error, L"Error initializing GUI Debugger", MB_OK);
		unloadDebuggerDLL(true);
		return;
	}

	Debugger->ShowDebugger(SW_RESTORE);
	gDebuggerExited = false;
}

void unloadDebuggerDLL(bool force)
{
	if (gDebuggerDll)
	{
		if (force)
		{
			Debugger->CloseDebugger();
			gDebuggerExited = true;
		}

		if (!gDebuggerExited)
			return;
		else
		{
			FreeLibrary(gDebuggerDll);
			gDebuggerDll = NULL;
			gDebuggerExited = true;
		}
	}
}

static void DebuggerExited(void)
{
	gDebuggerExited = true;
}

static size_t DbgMemoryRead(size_t address, DbgByte *buffer, size_t bufferSize)
{
	DbgByte *s = buffer;
	DbgByte *e = buffer+ bufferSize;

	uint8_t c = (address & 0xff);
	while (s < e)
	{
		s->value = c++;
		s->safe = true;
		s++;
	}

	return bufferSize;
}

static size_t DbgMemoryWrite(size_t address, DbgByte *buffer, size_t bufferSize)
{
	return bufferSize;
}

void initAPI()
{
	DEBUGGER_DLL_PROC_IMPORT(gDebuggerDll, InitDebugger);

	Debugger->DebuggerExited = DebuggerExited;
	Debugger->MemoryRead = DbgMemoryRead;
	Debugger->MemoryWrite = DbgMemoryWrite;

	// We use this dummy so that we can savely call the functions without
	// constantly checking the pointers. Dummy always returns 0.
	DEFAULT_FUNCTION(ShowDebugger);
	DEFAULT_FUNCTION(CloseDebugger);
}
