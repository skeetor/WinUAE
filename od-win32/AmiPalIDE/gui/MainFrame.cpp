
#include "framework.h"

#include "gui/AmiPalIDE.h"
#include "gui/MainFrame.h"
#include "gui/DocumentPanel.h"
#include "gui/MemoryPanel.h"
#include "gui/MemoryToolBar.h"

#include "gui/properties/ConfigDlg.h"

#include "config/ApplicationConfig.h"

#include <wx/app.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/window.h>
#include <wx/artprov.h>

using namespace std;

wxTextCtrl* MainFrame::CreateTextCtrl(const wxString& ctrl_text)
{
	static int n = 0;

	wxString text;
	if ( !ctrl_text.empty() )
		text = ctrl_text;
	else
		text.Printf("This is text box %d", ++n);

	return new wxTextCtrl(this, wxID_ANY, text,
		wxPoint(0,0), FromDIP(wxSize(150,90)),
		wxNO_BORDER | wxTE_MULTILINE);
}

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
, m_documents(nullptr)
, m_registerPanel(nullptr)
, m_memoryPanel(nullptr)
, m_memoryToolBar(nullptr)
, m_disasmPanel(nullptr)
, m_breakpointPanel(nullptr)
, m_consolePanel(nullptr)
, m_modalDialog(nullptr)
, m_closeByMenu(false)
{
	// Enable docking
	m_manager.SetManagedWindow(this);
	m_manager.SetFlags(wxAUI_MGR_DEFAULT);

	m_statusBar = CreateStatusBar(1, wxSTB_SIZEGRIP, IDM_STATUSBAR);
	m_frameMenu = new wxMenuBar(0);

	m_frameMenu->Append(createFileMenu(), "File");
	m_frameMenu->Append(createDebugMenu(), "Debug");
	m_frameMenu->Append(createViewMenu(), "View");
	m_frameMenu->Append(createToolsMenu(), "Tools");
	m_frameMenu->Append(createHelpMenu(), "Help");

	wxSize client_size = GetClientSize();
	m_documents = new DocumentPanel(this, wxID_ANY,
		wxPoint(client_size.x, client_size.y),
		FromDIP(wxSize(430,200)),
		wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_MIDDLE_CLICK_CLOSE);

	// nb->SetArtProvider(new wxAuiSimpleTabArt);

	m_manager.AddPane(m_documents, wxAuiPaneInfo().Name("notebook_content").CenterPane().PaneBorder(false));
	m_manager.AddPane(CreateFileBrowser(), wxAuiPaneInfo().Name("filebrowser").Caption("File Browser").Left().Layer(1).Position(1).CloseButton(true).MaximizeButton(true));

	MemoryToolBar *mbar = getMemoryToolBar();
	mbar->Disable();

	m_manager.AddPane(mbar, wxAuiPaneInfo().Name("MemoryBar").Caption("Memory Toolbar").ToolbarPane().Top().Row(1));
	createMemoryPanel();

	// "commit" all changes made to wxAuiManager
	m_manager.Update();

	SetMenuBar(m_frameMenu);

	// set the frame icon
	SetIcon(wxICON(sample));

	SetStatusText("Welcome to wxWidgets!");
}

MainFrame::~MainFrame(void)
{
}

MainFrame *MainFrame::getInstance(void)
{
	return wxGetApp().m_mainFrame;
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

wxTreeCtrl *MainFrame::CreateFileBrowser()
{
	wxTreeCtrl* tree = new wxTreeCtrl(this, wxID_ANY,
		wxPoint(0, 0),
		FromDIP(wxSize(160, 250)),
		wxTR_DEFAULT_STYLE | wxNO_BORDER);

	wxSize size = FromDIP(wxSize(16, 16));
	wxImageList* imglist = new wxImageList(size.x, size.y, true, 2);
	imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, size));
	imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, size));
	tree->AssignImageList(imglist);

	wxTreeItemId root = tree->AddRoot("wxAUI Project", 0);
	wxArrayTreeItemIds items;



	items.Add(tree->AppendItem(root, "Item 1", 0));
	items.Add(tree->AppendItem(root, "Item 2", 0));
	items.Add(tree->AppendItem(root, "Item 3", 0));
	items.Add(tree->AppendItem(root, "Item 4", 0));
	items.Add(tree->AppendItem(root, "Item 5", 0));


	int i, count;
	for (i = 0, count = items.Count(); i < count; ++i)
	{
		wxTreeItemId id = items.Item(i);
		tree->AppendItem(id, "Subitem 1", 1);
		tree->AppendItem(id, "Subitem 2", 1);
		tree->AppendItem(id, "Subitem 3", 1);
		tree->AppendItem(id, "Subitem 4", 1);
		tree->AppendItem(id, "Subitem 5", 1);
	}

	tree->Expand(root);

	return tree;
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
	createMemoryPanel();
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
	bool shown = m_memoryToolBar->IsShown();
	m_memoryToolBar->Show(!shown);

	// In case the toolbar was floating and has been closed by the user, we need
	// to make sure that it becomes visible, so we always detach, and insert it
	// to make it properly appear again.
	// It might be more efficient, if we could get an event when the toolbar has
	// been closed.
	m_manager.DetachPane(m_memoryToolBar);

	if (!shown)
		m_manager.InsertPane(m_memoryToolBar, wxAuiPaneInfo().Name("MemoryBar").Caption("Memory Toolbar").ToolbarPane().Top().Row(1));

	m_manager.Update();
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

RegistersPanel *MainFrame::createRegistersPanel(void)
{
	return CreateTextCtrl("RegistersPanel");
}

MemoryPanel *MainFrame::createMemoryPanel(void)
{
	MemoryPanel *mp = new MemoryPanel(getMemoryToolBar(), this);
	m_documents->AddPage(mp, "Memory", false);
	m_documents->SetSelection(m_documents->GetPageCount()-1);

	return mp;
}

MemoryToolBar *MainFrame::getMemoryToolBar(void)
{
	if (!m_memoryToolBar)
		m_memoryToolBar = new MemoryToolBar(this, this);

	return m_memoryToolBar;
}

DisasmPanel *MainFrame::createDisasmPanel(void)
{
	return CreateTextCtrl("DisasmPanel");
}

BreakpointPanel *MainFrame::createBreakpointPanel(void)
{
	return CreateTextCtrl("BreakpointPanel");
}

ConsolePanel *MainFrame::createConsolePanel(void)
{
	return CreateTextCtrl("ConsolePanel");
}
