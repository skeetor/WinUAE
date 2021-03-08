#include "config/ApplicationConfig.h"

#include "wx/confbase.h"

namespace
{
	ApplicationConfig gAppConfig;
}

ApplicationConfig::ApplicationConfig()
: Config("Application")
, closeAction(CloseButtonAction::CLOSE_WINDOW)
, saveLayout(true)
, savePosition(true)
, layout(wxT("default"))
{}

ApplicationConfig::~ApplicationConfig()
{
}

ApplicationConfig &ApplicationConfig::getInstance(void)
{
	return gAppConfig;
}

void ApplicationConfig::update(const ApplicationConfig &src)
{
	closeAction = src.closeAction;
	configFile = src.configFile;
	logFile = src.logFile;
	layout = src.layout;
	saveLayout = src.saveLayout;
	savePosition = src.savePosition;

	notify();
}

bool ApplicationConfig::serialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/" + getConfigName());

	config->Write("FrameSize", savePosition);
	config->Write("PerspectiveSave", saveLayout);
	config->Write("PerspectiveName", layout);
	config->Write("Logfile", logFile);

	return true;
}

bool ApplicationConfig::deserialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/" + getConfigName());

	savePosition = config->ReadBool("FrameSize", true);
	saveLayout = config->ReadBool("PerspectiveSave", true);
	config->Read("PerspectiveName", layout);
	config->Read("Logfile", logFile);

	return true;
}
