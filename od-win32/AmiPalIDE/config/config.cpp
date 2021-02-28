#include "config/ApplicationConfig.h"
#include "config/DebuggerConfig.h"

namespace
{
	// The font can not be initialized during static initialization, so we have to defer it
	// unitl some component requests it.
	bool gFontInitialize = true;

	ApplicationConfig gAppConfig;
	DebuggerConfig gDebuggerConfig;
}

ApplicationConfig &ApplicationConfig::getInstance(void)
{
	return gAppConfig;
}

DebuggerConfig &DebuggerConfig::getInstance(void)
{
	if (gFontInitialize)
	{
		gFontInitialize = false;
		gDebuggerConfig.memoryViewFont = wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New"));
	}

	return gDebuggerConfig;
}
