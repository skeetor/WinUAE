#pragma once

#include <wx/app.h>

class MainFrame;

class AmiPalApp : public wxApp
{
public:
	AmiPalApp(void);

	bool OnInit(void) override;
	int OnExit(void) override;

	bool parseCommandLine(int argc, wchar_t **argv);
	bool showCmdLineHelp(void);

	MainFrame *m_mainFrame;

	std::wstring configFile;
};

wxDECLARE_APP(AmiPalApp);
