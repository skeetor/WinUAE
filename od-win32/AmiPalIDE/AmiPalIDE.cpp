
#include "framework.h"

#include "AmiPalIDE.h"
#include "gui/MainFrame.h"
#include "utils/commandline.h"
#include "utils/exceptions.h"

#include "config/ApplicationConfig.h"

#include <wx/confbase.h>
#include <wx/msgdlg.h>

#include <stdexcept>

using namespace std;

//wxIMPLEMENT_APP(AmiPalApp);
extern AmiPalApp *gApp;

AmiPalApp::AmiPalApp(void)
: m_mainFrame(nullptr)
{
}

bool AmiPalApp::OnInit(void)
{
	gApp = nullptr;
	ApplicationConfig &config = ApplicationConfig::getInstance();

	configByParam = false;
	config.configFile = config.getDefaultConfigFile();

	if (!parseCommandLine(argc, argv))
		return false;

	wxConfigBase::DontCreateOnDemand();

//	if (!wxApp::OnInit())
//		return false;

	config.configFile = configFile;

	try
	{
		m_mainFrame = new MainFrame(_T("AmiPalIDE v0.01"));
		m_mainFrame->Show(true);
	}
	catch (SerializeException const &ex)
	{
		wxString s;

		s += "Invalid configuration in file: " + config.configFile + "\n\n";
		s += ex.what();
		s += ex.getKey() + " = " + ex.getValue() + "\n";

		wxMessageBox(s, "Error reading config file!", wxOK);
		return false;
	}
	catch (const SilentException &ex)
	{
		UNUSED(ex);
		return false;
	}

	gApp = this;

	return true;
}

int AmiPalApp::OnExit(void)
{
	if(m_mainFrame)
		m_mainFrame->Close(true);

	return 0;
}

bool AmiPalApp::showCmdLineHelp(void)
{
	return false;
}

bool AmiPalApp::parseCommandLine(int argc, wchar_t **argv)
{
	vector<wstring> params;

	if (findParam(argc, argv, L"config", params) != -1)
	{
		if (params.size() != 1)
			return showCmdLineHelp();

		configFile = params[0];
		if (configFile.empty())
			configFile = ApplicationConfig::getInstance().getDefaultConfigFile();
		else
			configByParam = true;
	}

	return true;
}
