#include "config/ApplicationConfig.h"

#include "wx/confbase.h"
#include <wx/stdpaths.h>

namespace
{
	ApplicationConfig gAppConfig;
}

ApplicationConfig::ApplicationConfig()
: Config("Application")
, closeAction(CloseButtonAction::CLOSE_WINDOW)
, saveLayout(true)
, savePosition(true)
, configFile("")
, layout(wxT("default"))
{
}

ApplicationConfig::~ApplicationConfig()
{
}

ApplicationConfig &ApplicationConfig::getInstance(void)
{
	return gAppConfig;
}

wxString ApplicationConfig::getDefaultConfigFile(void) const
{
	return wxStandardPaths::Get().GetUserConfigDir() + "\\" + APPLICATION_NAME + ".cfg";
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
	layout = config->Read("PerspectiveName", "default");
	logFile = config->Read("Logfile", "");

	notify();

	return true;
}
