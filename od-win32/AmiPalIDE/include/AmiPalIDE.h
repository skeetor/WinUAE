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

	// This is an internal flag to indicate if the configfile was provided
	// via argument by the user, or if it is a default name.
	// If it was provided by the user and we didn't find it, we show a
	// messagebox to the user. In the other case the GUI starts silently with
	// default settings (i.E. first time usage).
	bool configByParam : 1;
};

wxDECLARE_APP(AmiPalApp);
