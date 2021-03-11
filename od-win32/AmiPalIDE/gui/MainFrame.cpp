
#include "framework.h"

#include "AmiPalIDE.h"
#include "gui/MainFrame.h"
#include "gui/properties/ConfigDlg.h"
#include "gui/document/DocumentManager.h"
#include "gui/document/MemoryToolBar.h"
#include "gui/document/FileTree.h"
#include "gui/document/panels/DocumentPanel.h"
#include "gui/document/panels/MemoryPanel.h"

#include "config/ApplicationConfig.h"
#include "config/DebuggerConfig.h"

#include "utils/StringUtils.h"
#include "utils/exceptions.h"
#include "debugger/DebuggerAPI.h"

#include <wx/app.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/window.h>
#include <wx/artprov.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/treectrl.h>

using namespace std;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_CLOSE(MainFrame::OnClose)

	EVT_MENU(IDM_QUIT, MainFrame::OnQuit)
	EVT_MENU(IDM_ABOUT, MainFrame::OnAbout)

	EVT_MENU(IDM_DEBUG_REGISTERS, MainFrame::OnRegisters)
	EVT_MENU(IDM_DEBUG_DISASM, MainFrame::OnDisasm)
	EVT_MENU(IDM_DEBUG_MEMORY, MainFrame::OnMemory)
	EVT_MENU(IDM_DEBUG_BREAKPOINTS, MainFrame::OnBreakpoints)
	EVT_MENU(IDM_DEBUG_CONSOLE, MainFrame::OnConsole)

	EVT_MENU(IDM_VIEW_MEMORY_TOOLBAR, MainFrame::OnViewMemoryToolbar)

	EVT_MENU(IDM_TOOLS_OPTIONS, MainFrame::OnOptions)
	EVT_MENU(IDM_TOOLS_LAYOUT_SAVE, MainFrame::OnLayoutSave)
	EVT_MENU(IDM_TOOLS_LAYOUT_LOAD, MainFrame::OnLayoutLoad)

wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
: wxFrame(NULL, wxID_ANY, title)
, m_statusBar(nullptr)
, m_frameMenu(nullptr)
, m_modalDialog(nullptr)
, m_closeByMenu(false)
, m_abort(false)
{
	wxGetApp().m_mainFrame = this;

	init();
	restoreConfig();

	SetStatusText("Welcome to the AmiPal Debugger!");
}

MainFrame::~MainFrame(void)
{
	delete m_manager;
}

MainFrame *MainFrame::getInstance(void)
{
	return wxGetApp().m_mainFrame;
}

void MainFrame::init(void)
{
	m_manager = new DocumentManager(this, wxAUI_MGR_DEFAULT);

	m_statusBar = CreateStatusBar(1, wxSTB_SIZEGRIP, IDM_STATUSBAR);
	m_frameMenu = new wxMenuBar(0);

	m_frameMenu->Append(createFileMenu(), "File");
	m_frameMenu->Append(createDebugMenu(), "Debug");
	m_frameMenu->Append(createViewMenu(), "View");
	m_frameMenu->Append(createToolsMenu(), "Tools");
	m_frameMenu->Append(createHelpMenu(), "Help");
	SetMenuBar(m_frameMenu);

	// set the frame icon
	SetIcon(wxICON(sample));
}

void MainFrame::createDefaultIDE(void)
{
	wxSize client_size = GetClientSize();
	DocumentPanel *documents = new DocumentPanel(this, wxID_ANY,
									wxPoint(client_size.x, client_size.y),
									FromDIP(wxSize(430, 200)),
									wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_MIDDLE_CLICK_CLOSE);

	// m_documents->SetArtProvider(new wxAuiSimpleTabArt);
	MemoryToolBar *mbar = getMemoryToolBar();
	mbar->Disable();

	MemoryPanel *mp = CREATE_DOCUMENT(MemoryPanel, documents);

	documents->AddPage(mp, "Memory", true);
	documents->SetSelection(documents->GetPageCount() - 1);

	m_manager->AddPane(documents, wxAuiPaneInfo().Name("DocumentPanel").CenterPane().PaneBorder(false));
	m_manager->AddPane(CREATE_DOCUMENT_WINDOW(FileTree, this), wxAuiPaneInfo().Name(STRINGIFY(FileTree)).Caption("File Browser").Left().Layer(1).Position(1).CloseButton(true).MaximizeButton(true));

	// "commit" all changes made to wxAuiManager
	m_manager->Update();
}

wxMenu *MainFrame::createFileMenu(void)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item;

	item = new wxMenuItem(menu, IDM_QUIT, wxString(wxT("E&xit")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu *MainFrame::createDebugMenu(void)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item;

	item = new wxMenuItem(menu, IDM_DEBUG_REGISTERS, wxString(wxT("Registers")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, IDM_DEBUG_DISASM, wxString(wxT("Dissassembly")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, IDM_DEBUG_MEMORY, wxString(wxT("Memory")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, IDM_DEBUG_BREAKPOINTS, wxString(wxT("Breakpoints")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, IDM_DEBUG_CONSOLE, wxString(wxT("Console")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu *MainFrame::createHelpMenu(void)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item;

	item = new wxMenuItem(menu, IDM_ABOUT, wxString(wxT("&About")), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu *MainFrame::createViewMenu(void)
{
	wxMenuItem *item;
	wxMenu *viewMenu = new wxMenu();

	// Toolbars
	wxMenu *toolbarMenu = new wxMenu();
	item = new wxMenuItem(viewMenu, wxID_ANY, wxT("Toolbars"), wxEmptyString, wxITEM_NORMAL, toolbarMenu);
	viewMenu->Append(item);

	item = new wxMenuItem(toolbarMenu, IDM_VIEW_MEMORY_TOOLBAR, wxString(wxT("Memory")), wxEmptyString, wxITEM_NORMAL);
	toolbarMenu->Append(item);

	return viewMenu;
}

wxMenu *MainFrame::createToolsMenu(void)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item;

	item = new wxMenuItem(menu, IDM_TOOLS_LAYOUT_SAVE, wxString(wxT("Save Layout...")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, IDM_TOOLS_LAYOUT_LOAD, wxString(wxT("Load Layout...")) , wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, IDM_TOOLS_OPTIONS, wxString(wxT("&Options")), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	m_closeByMenu = true;

	Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(wxString::Format
	(
		"Welcome to %s!\n"
		"\n"
		"This is the minimal wxWidgets sample\n"
		"running under %s.",
		wxVERSION_STRING,
		wxGetOsDescription()
	),
		"About wxWidgets minimal sample",
		wxOK | wxICON_INFORMATION,
		this);
}

void MainFrame::OnClose(wxCloseEvent &event)
{
	if (!m_abort)
	{
		if (event.CanVeto())
		{
			switch (ApplicationConfig::getInstance().closeAction)
			{
				case CLOSE_IGNORE:
					event.Veto();
				return;

				case CLOSE_MINIMIZE:
					Iconize();
					event.Veto();
				return;
			}
		}

		if (m_modalDialog)
			m_modalDialog->EndModal(wxID_CANCEL);

		saveConfig();
	}

	Debugger->DebuggerExited();

	// Disable close on exit.
	wxGetApp().m_mainFrame = nullptr;

	// Process the event to allow the close.
	event.Skip();
}

void MainFrame::OnRegisters(wxCommandEvent &event)
{
	//m_documents->AddPage(createRegistersPanel(), "CPU", false);
}

void MainFrame::OnDisasm(wxCommandEvent& event)
{
	//m_documents->AddPage(createDisasmPanel(), "Disasm", false);
}

void MainFrame::OnMemory(wxCommandEvent& event)
{
	DocumentPanel *p = getDocumentPanel();

	p->AddPage(CREATE_DOCUMENT(MemoryPanel, p), "Memory", true);
}

void MainFrame::OnBreakpoints(wxCommandEvent& event)
{
	//m_documents->AddPage(createBreakpointPanel(), "Breakpoints", false);
}

void MainFrame::OnConsole(wxCommandEvent& event)
{
	//m_documents->AddPage(createConsolePanel(), "Console", false);
}

void MainFrame::OnViewMemoryToolbar(wxCommandEvent &event)
{
	MemoryToolBar *m = getMemoryToolBar();
	bool shown = m->IsShown();
	m->Show(!shown);

	// In case the toolbar was floating and has been closed by the user, we need
	// to make sure that it becomes visible, so we always detach, and insert it
	// to make it properly appear again.
	// It might be more efficient, if we could get an event when the toolbar has
	// been closed.
	m_manager->DetachPane(m);

	if (!shown)
		m_manager->InsertPane(m, wxAuiPaneInfo().Name("MemoryBar").Caption("Memory Toolbar").ToolbarPane().Top().Row(1));

	m_manager->Update();
}

void MainFrame::OnOptions(wxCommandEvent& event)
{
	ConfigDlg dlg(this);

	// We don't really care about the exit code. The user
	// must press apply to change the settings, and after that
	// there is no going back, so canceling the dialog has no
	// effect in this case. Only if the user did not press apply
	// but then, the changes are discarded anyway.
	dlg.ShowModal();
}

void MainFrame::OnLayoutSave(wxCommandEvent& event)
{
}

void MainFrame::OnLayoutLoad(wxCommandEvent& event)
{
}

MemoryToolBar *MainFrame::getMemoryToolBar(void)
{
	MemoryToolBar *w = reinterpret_cast<MemoryToolBar *>(m_manager->GetPane("MemoryToolBar").window);

	if (!w)
	{
		w = CREATE_DOCUMENT_WINDOW(MemoryToolBar, this);
		//w = DocumentWindow::createFromInfo<MemoryToolBar>(this);
		m_manager->AddPane(w, wxAuiPaneInfo().Name("MemoryToolBar").Caption("Memory Toolbar").ToolbarPane().Top().Row(1));
	}

	return w;
}

DocumentPanel *MainFrame::getDocumentPanel(void)
{
	DocumentPanel *p = reinterpret_cast<DocumentPanel *>(m_manager->GetPane("DocumentPanel").window);
	if (!p)
	{
		p = CREATE_DOCUMENT_WINDOW(DocumentPanel, this);
		m_manager->AddPane(p, wxAuiPaneInfo().Name("DocumentPanel").CenterPane().PaneBorder(false));
	}

	return p;
}

bool MainFrame::serialize(wxString const &groupId, wxConfigBase *config)
{
	ApplicationConfig &appCfg = ApplicationConfig::getInstance();
	appCfg.serialize("", config);

	DebuggerConfig::getInstance().serialize("", config);

	config->SetPath("/GlobalSettings");

	if (appCfg.savePosition)
	{
		bool maximized = IsMaximized();

		if (maximized)
			Maximize(false);

		int fx, fy, fw, fh;
		GetPosition(&fx, &fy);
		GetSize(&fw, &fh);
		config->Write("FrameX", fx);
		config->Write("FrameY", fy);
		config->Write("FrameW", fw);
		config->Write("FrameH", fh);
		config->Write("FrameMaximized", maximized);
	}

	if (appCfg.saveLayout)
		m_manager->serialize("", config);

	return true;
}

bool MainFrame::deserialize(wxString const &groupId, wxConfigBase *config)
{
	ApplicationConfig &appCfg = ApplicationConfig::getInstance();
	appCfg.deserialize("", config);

	DebuggerConfig::getInstance().deserialize("", config);

	config->SetPath("/GlobalSettings");

	if (appCfg.savePosition)
	{
		bool maximized = config->ReadBool("FrameMaximized", false);
		Maximize(false);

		int w, h;
		wxPoint p;

		config->Read("FrameX", &p.x);
		config->Read("FrameY", &p.y);
		w = ((config->Read("FrameW", &w) != false) * w);
		h = ((config->Read("FrameH", &h) != false) * w);

		SetPosition(p);
		if (w != 0 && h != 0)
			SetSize(w, h);

		if (maximized)
			Maximize(true);
	}

	if (appCfg.saveLayout)
		m_manager->deserialize("", config);

	return true;
}

void MainFrame::saveConfig(void)
{
	ApplicationConfig &appCfg = ApplicationConfig::getInstance();
	wxString fn = appCfg.configFile;
	if (fn.empty())
	{
		fn = wxStandardPaths::Get().GetUserConfigDir();
	}

	wxFileOutputStream ostrm(fn);
	if (!ostrm.Ok())
	{
		string msg = string("Unable to open file: ") + string(fn.c_str());
		// TODO: error handling
	}

	wxFileConfig *config = new wxFileConfig();
	wxConfigBase::Set(config);

	serialize("frame", config);

	if (!config->Save(ostrm))
	{
		// TODO: error handling
	}
	ostrm.Close();

	wxConfigBase::Set(nullptr);
	delete config;
}

void MainFrame::restoreConfig(void)
{
	ApplicationConfig &appCfg = ApplicationConfig::getInstance();
	wxString fn = appCfg.configFile;
	if (fn.empty())
	{
		wxGetApp().configByParam = false;
		fn = appCfg.getDefaultConfigFile();
		appCfg.configFile = fn;
	}

	if (!wxFile::Exists(fn))
	{
		if (wxGetApp().configByParam)
		{
			int rc = wxMessageBox(wxString("Unable to open file: ") + fn + wxT(" for reading! Abort?"), "Error opening config file!", wxYES_NO);
			if (rc == wxYES)
			{
				m_abort = true;
				string msg = string("Unable to open config file: ") + string(fn.c_str());
				throw SilentException(msg);
			}
		}

		createDefaultIDE();

		return;
	}

	wxFileInputStream istrm(fn);
	if (!istrm.Ok())
	{
		string msg = string("Unable to open config file: ") + string(fn.c_str());
		throw invalid_argument(msg);
	}

	wxFileConfig *config = new wxFileConfig(istrm);
	wxConfigBase::Set(config);

	deserialize("", config);

	delete config;
}
