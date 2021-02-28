#pragma once

#include "config/Config.h"
#include <wx/font.h>

class DebuggerConfig
: public Config<DebuggerConfig>
{
public:
	DebuggerConfig()
	: askOnDeleteAllBreakpoints(false)
	, memoryViewFont()
	{
	}

	~DebuggerConfig() override
	{
	}

	static DebuggerConfig &getInstance(void);

	void update(const DebuggerConfig &src) override
	{
		askOnDeleteAllBreakpoints = src.askOnDeleteAllBreakpoints;
		memoryViewFont = src.memoryViewFont;

		notify();
	}

public:
	bool askOnDeleteAllBreakpoints;
	wxFont memoryViewFont;
};
