#pragma once

#include "config/Config.h"
#include <wx/string.h>

#define APPLICATION_NAME wxT("AmiPal")

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
	, savePosition(true)
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
		savePosition = src.savePosition;

		notify();
	}

public:
	CloseButtonAction closeAction;
	bool saveLayout:1;
	bool savePosition:1;

	wxString configFile;
	wxString logFile;
	wxString layout;
};
