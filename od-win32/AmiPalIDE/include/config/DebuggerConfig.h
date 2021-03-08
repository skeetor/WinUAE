#pragma once

#include "config/Config.h"
#include <wx/font.h>

class DebuggerConfig
: public Config<DebuggerConfig>
{
public:
	DebuggerConfig(void);
	~DebuggerConfig() override;

	static DebuggerConfig &getInstance(void);

	void update(const DebuggerConfig &src);

	bool serialize(wxString const &groupId, wxConfigBase *config) override;
	bool deserialize(wxString const &groupId, wxConfigBase *config) override;

public:
	wxFont memoryViewFont;
	bool askOnDeleteAllBreakpoints;
};
