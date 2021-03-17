
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

MainFrame::MainFrame(void)
: wxFrame(NULL, wxID_ANY, _T("AmiPalIDE v0.01"))
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
	MainFrame *frame = wxGetApp().m_mainFrame;
	if (!frame)
	{
		frame = new MainFrame();
		wxGetApp().m_mainFrame = frame;
	}

	return frame;
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
	SetSize(1024,600);

	DocumentPanel *documents = CREATE_DOCUMENT_WINDOW(DocumentPanel, this);

	// documents->SetArtProvider(new wxAuiSimpleTabArt);
	MemoryToolBar *mbar = getMemoryToolBar();
	mbar->Disable();

	m_manager->AddPane(documents, wxAuiPaneInfo().Name("DocumentPanel").CenterPane().PaneBorder(false));
	newMemoryPanel(true);
	newMemoryPanel();
	newMemoryPanel();
	newMemoryPanel();
/*	newMemoryPanel();
	newMemoryPanel();
	newMemoryPanel();
	newMemoryPanel();*/

	m_manager->AddPane(CREATE_DOCUMENT_WINDOW(FileTree, this), wxAuiPaneInfo().Name(STRINGIFY(FileTree)).Caption("File Browser").Left().Layer(1).Position(1).CloseButton(true).MaximizeButton(true));

	// "commit" all changes made to wxAuiManager
	m_manager->Update();

	documents->Split(documents->GetPageCount() - 2, wxRIGHT);
	documents->Split(documents->GetPageCount() - 1, wxRIGHT);
	wxString perspective = documents->SerializeLayout();

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

void MainFrame::newMemoryPanel(bool select)
{
	DocumentPanel *p = getDocumentPanel();

	MemoryPanel *m = CREATE_DOCUMENT(MemoryPanel, p);
	p->AddPage(m, m->getTitle(), select);
}

void MainFrame::OnMemory(wxCommandEvent& event)
{
	newMemoryPanel(true);
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
	DocumentPanel *p = getDocumentPanel();

	m_perspective = m_manager->SavePerspective();
	m_perspectivePanel = p->SerializeLayout();

	SetStatusText("Perspective saved...");
}

void MainFrame::OnLayoutLoad(wxCommandEvent& event)
{
	DocumentPanel *p = getDocumentPanel();
	wxAuiManager *m = p->GetManager();

	p->DeserializeLayout(m_perspectivePanel);
	m_manager->LoadPerspective(m_perspective);
	m_manager->Update();

	SetStatusText("Perspective loaded...");
}

MemoryToolBar *MainFrame::getMemoryToolBar(void)
{
	MemoryToolBar *w = reinterpret_cast<MemoryToolBar *>(m_manager->GetPane("MemoryToolBar").window);

	if (!w)
	{
		w = CREATE_DOCUMENT_WINDOW(MemoryToolBar, this);
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

bool MainFrame::serialize(wxString groupId, wxConfigBase *config)
{
	ApplicationConfig &appCfg = ApplicationConfig::getInstance();
	appCfg.serialize("", config);

	DebuggerConfig::getInstance().serialize("", config);

	config->SetPath("/GlobalSettings");

	bool maximized = IsMaximized();

	if (maximized)
		Maximize(false);

	wxPoint p = GetPosition();
	wxSize sz = GetSize();
	p = ToDIP(p);
	sz = ToDIP(sz);

	config->Write("FrameX", p.x);
	config->Write("FrameY", p.y);
	config->Write("FrameW", sz.x);
	config->Write("FrameH", sz.y);
	config->Write("FrameMaximized", maximized);

	m_manager->serialize("", config);

	return true;
}

bool MainFrame::deserialize(wxString groupId, wxConfigBase *config)
{
	ApplicationConfig &appCfg = ApplicationConfig::getInstance();
	appCfg.deserialize("", config);

	DebuggerConfig::getInstance().deserialize("", config);

	config->SetPath("/GlobalSettings");

	bool maximized = config->ReadBool("FrameMaximized", false);
	Maximize(false);

	wxPoint p;
	wxSize sz;

	config->Read("FrameX", &p.x);
	config->Read("FrameY", &p.y);
	p = FromDIP(p);

	config->Read("FrameW", &sz.x);
	config->Read("FrameH", &sz.y);
	sz = FromDIP(sz);

	SetPosition(p);
	if (sz.x != 0 && sz.y != 0)
		SetSize(sz);

	if (maximized)
		Maximize(true);

	m_manager->deserialize("", config);
	m_manager->Update();
	Update();

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

	//if (!wxFile::Exists(fn))
	{
		/*if (wxGetApp().configByParam)
		{
			int rc = wxMessageBox(wxString("Unable to open file: ") + fn + wxT(" for reading! Abort?"), "Error opening config file!", wxYES_NO);
			if (rc == wxYES)
			{
				m_abort = true;
				string msg = string("Unable to open config file: ") + string(fn.c_str());
				throw SilentException(msg);
			}
		}*/

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
