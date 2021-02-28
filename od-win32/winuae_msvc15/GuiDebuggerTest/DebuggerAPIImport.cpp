#include <string>
#include <iostream>

#include <Windows.h>

#define DEBUGGER_DLL_PROC_IMPL
#include "debugger/DebuggerAPI.h"
void loadDebuggerDLL(const TCHAR *dllName, TCHAR *cmdLine);
void unloadDebuggerDLL(bool bForce);


using namespace std;

static HMODULE gDebuggerDll = NULL;
static bool gDebuggerExited = true;

static void initAPI()
{
	DEBUGGER_DLL_PROC_IMPORT(gDebuggerDll, InitDebugger);
	DEBUGGER_DLL_PROC_IMPORT(gDebuggerDll, CloseDebugger);
	DEBUGGER_DLL_PROC_IMPORT(gDebuggerDll, ShowDebugger);

	DEBUGGER_CALLBACK_PROC_IMPORT(gDebuggerDll, DebuggerExited);
	DEBUGGER_CALLBACK_PROC_IMPORT(gDebuggerDll, DbgMemoryRead);
	DEBUGGER_CALLBACK_PROC_IMPORT(gDebuggerDll, DbgMemoryWrite);
}

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
			ShowDebugger(SW_RESTORE);
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
	if ((error = InitDebugger(getVersion(), &cmd[0])) != nullptr)
	{
		MessageBoxW(NULL, error, L"Error initializing GUI Debugger", MB_OK);
		unloadDebuggerDLL(true);
		return;
	}

	ShowDebugger(SW_RESTORE);
	gDebuggerExited = false;
}

void unloadDebuggerDLL(bool force)
{
	if (gDebuggerDll)
	{
		if (force)
		{
			CloseDebugger();
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
