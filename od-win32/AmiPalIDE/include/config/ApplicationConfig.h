#pragma once

#include "config/Config.h"
#include <wx/string.h>

enum CloseButtonAction
{
	CLOSE_WINDOW,
	CLOSE_MINIMIZE,
	CLOSE_IGNORE
};

class ApplicationConfig
: public Config<ApplicationConfig>
{
public:
	ApplicationConfig()
	: closeAction(CloseButtonAction::CLOSE_WINDOW)
	, saveLayout(true)
	, layout(wxT("default"))
	{}

	~ApplicationConfig() override
	{}

	static ApplicationConfig &getInstance(void);

	void update(const ApplicationConfig &src) override
	{
		closeAction = src.closeAction;
		configFile = src.configFile;
		logFile = src.logFile;
		layout = src.layout;
		saveLayout = src.saveLayout;

		notify();
	}

public:
	CloseButtonAction closeAction;
	bool saveLayout:1;

	wxString configFile;
	wxString logFile;
	wxString layout;
};
