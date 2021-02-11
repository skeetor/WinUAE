
#include "framework.h"

#include "gui/AmiPalIDE.h"
#include "gui/MainFrame.h"
#include "utils/commandline.h"

using namespace std;

//wxIMPLEMENT_APP(AmiPalApp);
extern AmiPalApp *gApp;

AmiPalApp::AmiPalApp(void)
: m_mainFrame(nullptr)
{
}

bool AmiPalApp::OnInit(void)
{
	if (!parseCommandLine(argc, argv))
		return false;

//	if (!wxApp::OnInit())
//		return false;

	m_mainFrame = new MainFrame(_T("AmiPalIDE v0.01"), configFile);
	m_mainFrame->Show(true);

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
	}

	return true;
}
