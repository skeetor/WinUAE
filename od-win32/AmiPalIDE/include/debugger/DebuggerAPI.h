#pragma once

#ifndef GUI_DEBUGGER_GLOBALS_H
#include "globals.h"
#include <stdint.h>
#include "Machine.h"
#endif

#ifdef WINUAE_DEBUGGER_API_EXPORTS
	#define WINUAE_DEBUGGER_API __declspec(dllexport)
#else // WINUAE_DEBUGGER_API_EXPORTS
	#define WINUAE_DEBUGGER_API __declspec(dllimport)
#endif // WINUAE_DEBUGGER_API_EXPORTS

#ifndef STRINGIFY
#define TO_STRING(a) STRINGIFY(a)
#define STRINGIFY(a) #a
#endif

#define MAJOR_VERSION	0x01
#define MINOR_VERSION	0x02
#define BUILD_VERSION	0x0304

static inline uint32_t getVersion(void)
{
	return ((MAJOR_VERSION << 24) | (MINOR_VERSION << 16) | BUILD_VERSION);
}

/**
 * DEBUGGER_DLL_PROC creates the definitions for functions which can be called
 * from the client into the DLL.
 *
 * Example: Client function call: void ShowDebugger();
 * DLL must implement the corresponding void ShowDebugger() function, which does
 * whatever is needed in the DLL.
 * 
 ********************************************************************************
 *
 * DEBUGGER_CALLBACK_PROC creates the definitions for functions which the debugger
 * calls from the DLL into the client, to get i.E. machine state, memory, etc.
 * 
 * Example: Debugger function call: uint32_t MemoryRead(...);
 * Client must implement the corresponding function. This function will be set in the DLL
 * so it can be used when the Debugger needs the required functionality.
 */

// ******************************************************************************
#ifdef WINUAE_DEBUGGER_API_EXPORTS

	// The DLL itself doesn't need the exported functions
	#define DEBUGGER_DLL_PROC(retType, name, ...)

#else	// WINUAE_DEBUGGER_API_EXPORTS

#ifdef DEBUGGER_DLL_PROC_IMPL

	// Generate global symbols for imported functions in the EXE
	#define DEBUGGER_DLL_PROC(retType, name, ...)\
		typedef WINUAE_DEBUGGER_API retType (*name##Proc)(__VA_ARGS__);\
		name##Proc name

#else // DEBUGGER_DLL_PROC_IMPL

	// Generate external global symbols for imported functions in the EXE
	#define DEBUGGER_DLL_PROC(retType, name, ...)\
		typedef WINUAE_DEBUGGER_API retType (*name##Proc)(__VA_ARGS__);\
		extern name##Proc name

#endif // DEBUGGER_DLL_PROC_IMPL

#endif	// WINUAE_DEBUGGER_API_EXPORTS

#define DEBUGGER_DLL_PROC_IMPORT(dllHandle, name)\
	name = (name##Proc)GetProcAddress(dllHandle, TO_STRING(name))

// ******************************************************************************
#ifdef WINUAE_DEBUGGER_API_EXPORTS

	#ifdef DEBUGGER_CALLBACK_PROC_IMPL

		// Generate the setters in the API module, to be exported by the DLL.
		// While the DLL is loading, the controls may already start to call
		// debugger functions. At this point, the DLL client had not yet a chance
		// to set the callback functions. Therfore we initialize the callbackpointers
		// with a dummies "Silent" function, which always returns 0, so the functions
		// can already be called without the need of checking if the debugger is attached.
		// As soon as the debugger is attached, the pointers will be reset to their
		// appropriate function.
		// NOTE: This means that callback functions should not return 0 as an 
		// important value, as it is assumed that this will cause no side effects.
		#define DEBUGGER_CALLBACK_PROC(retType, name, ...) \
			typedef retType (*name##Proc)(__VA_ARGS__); \
			extern "C" void *name##Silent(__VA_ARGS__)\
			{\
				return 0;\
			}\
			name##Proc name = (name##Proc)name##Silent;\
			extern "C" WINUAE_DEBUGGER_API void set##name(name##Proc p##name##Proc)\
			{\
				name = p##name##Proc;\
			}

	#else	// DEBUGGER_CALLBACK_PROC_IMPL

		// Generate the callback prototypes for DLL calls, so the debugger
		// can call them like normal functions.
		#define DEBUGGER_CALLBACK_PROC(retType, name, ...) \
			typedef retType (*name##Proc)(__VA_ARGS__);\
			extern name##Proc name

	#endif	// DEBUGGER_CALLBACK_PROC_IMPL

#else // WINUAE_DEBUGGER_API_EXPORTS

	// Generate the prototypes for the API importer
	// Function prototype, 
	#define DEBUGGER_CALLBACK_PROC(retType, name, ...) \
		retType name(__VA_ARGS__); \
		typedef retType (*name##Proc)(__VA_ARGS__); \
		typedef retType (*set##name##Proc)(name##Proc); \
		extern "C" WINUAE_DEBUGGER_API retType set##name(name##Proc)

#endif // WINUAE_DEBUGGER_API_EXPORTS

#define DEBUGGER_CALLBACK_PROC_IMPORT(dllHandle, name)\
	set##name##Proc set##name = (set##name##Proc)GetProcAddress(dllHandle, TO_STRING(set##name));\
	set##name(name)

// ******************************************************************************
// ****************** DLL functions to be used from the client ******************
// ******************************************************************************

/**
 * Initialize the application.
 * If the return value is not a nullptr it contains a wchar message with an error
 * description.
 */
DEBUGGER_DLL_PROC(wchar_t *, InitDebugger, uint32_t version, wchar_t *cmdLine);

/**
 * Closes teh debugger and exits the debugger application. This has to be called
 * before the FreeLibrary() is used on the DLL and the debugger has not yet exited.
 */
DEBUGGER_DLL_PROC(void, CloseDebugger);

/**
 * Show the debugger window.
 * 
 * @param nCmdShow : SW_RESTORE, SW_...
 */
DEBUGGER_DLL_PROC(void, ShowDebugger, int nCmdShow);


// ******************************************************************************
// *************** Callbackfunctions to be used from the Debugger ***************
// ******************************************************************************

/**
 * Called when the debugger has exited. The debugger can no longer be used after
 * this callback has been received and the DLL should be unloaded.
 * Note that the client should not unload the DLL from within this callback as the
 * debugger may still perform some internal processing.
 */
DEBUGGER_CALLBACK_PROC(void, DebuggerExited);

/**
 * Read memory from the specified address. Returns the number of bytes read.
 */
DEBUGGER_CALLBACK_PROC(size_t, DbgMemoryRead, size_t address, DbgByte *buffer, size_t bufferSize);

/**
 * Write memory to the specified address. Returns the number of bytes written.
 */
DEBUGGER_CALLBACK_PROC(size_t, DbgMemoryWrite, size_t address, DbgByte *buffer, size_t bufferSize);
