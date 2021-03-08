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
	ApplicationConfig();
	~ApplicationConfig() override;

	static ApplicationConfig &getInstance(void);

	void update(const ApplicationConfig &src);
	wxString getDefaultConfigFile(void) const;

public: // Serialize
	bool serialize(wxString const &groupId, wxConfigBase *config) override;
	bool deserialize(wxString const &groupId, wxConfigBase *config) override;

public:
	CloseButtonAction closeAction;
	bool saveLayout:1;
	bool savePosition:1;

	wxString configFile;
	wxString logFile;
	wxString layout;
};
