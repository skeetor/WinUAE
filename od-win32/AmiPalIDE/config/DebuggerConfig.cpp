#include "config/DebuggerConfig.h"

#include "wx/confbase.h"

namespace
{
	// The font can not be initialized during static initialization, so we have to defer it
	// until some component requests it.
	bool gFontInitialize = true;

	DebuggerConfig gDebuggerConfig;
}

DebuggerConfig::DebuggerConfig()
: Config("Debugger")
, memoryViewFont()
, askOnDeleteAllBreakpoints(false)
{
}

DebuggerConfig::~DebuggerConfig()
{
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

void DebuggerConfig::update(const DebuggerConfig &src)
{
	askOnDeleteAllBreakpoints = src.askOnDeleteAllBreakpoints;
	memoryViewFont = src.memoryViewFont;

	notify();
}


bool DebuggerConfig::serialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/"+getConfigName());

	config->Write("AskDeleteAllBreakpoints", askOnDeleteAllBreakpoints);
	config->Write("MemoryViewFont", memoryViewFont.GetNativeFontInfoDesc());

	return true;
}

bool DebuggerConfig::deserialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/" + getConfigName());

	askOnDeleteAllBreakpoints = config->ReadBool("AskDeleteAllBreakpoints", true);
	wxString s;
	config->Read("MemoryViewFont", s);
	memoryViewFont.SetNativeFontInfo(s);
	notify();

	return true;
}
