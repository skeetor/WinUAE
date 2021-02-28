#pragma once

#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>

#include "config/ApplicationConfig.h"

// TODO: Can be removed when the actual classes are implemented
typedef wxTextCtrl RegistersPanel;
typedef wxTextCtrl DisasmPanel;
typedef wxTextCtrl BreakpointPanel;
typedef wxTextCtrl ConsolePanel;

class wxDialog;

class MemoryPanel;
class MemoryToolBar;
class DocumentPanel;
/*class RegistersPanel;
class DisasmPanel;
class BreakpointPanel;
class ConsolePanel;*/

// IDs for the controls and the menu commands
enum AmiPalMainFrameID
{
	// Standard wx IDs
	IDM_QUIT = wxID_EXIT

	// it is important for the id corresponding to the "About" command to have
	// this standard value as otherwise it won't be handled properly under Mac
	// (where it is special and put into the "Apple" menu)
	,IDM_ABOUT = wxID_ABOUT

	// Not used
	,IDM_UNUSED = wxID_HIGHEST
 
	,IDM_STATUSBAR

	// Debugger Menu
	,IDM_DEBUG_REGISTERS
	,IDM_DEBUG_DISASM
	,IDM_DEBUG_MEMORY
	,IDM_DEBUG_BREAKPOINTS
	,IDM_DEBUG_CONSOLE

	// View Menu
	,IDM_VIEW_MEMORY_TOOLBAR

	// Tools Menu
	,IDM_TOOLS_OPTIONS
	,IDM_TOOLS_LAYOUT_SAVE
	,IDM_TOOLS_LAYOUT_LOAD
};

// Define a new frame type: this is going to be our main frame
class MainFrame
: public wxFrame
{
public:
	MainFrame(const wxString& title);
	~MainFrame(void) override;

	static MainFrame *getInstance(void);

	void OnClose(wxCloseEvent& event);

	// File Menu
	void OnQuit(wxCommandEvent& event);

	// Help Menu
	void OnAbout(wxCommandEvent& event);

	// Debug Menu
	void OnRegisters(wxCommandEvent& event);
	void OnDisasm(wxCommandEvent& event);
	void OnMemory(wxCommandEvent& event);
	void OnBreakpoints(wxCommandEvent& event);
	void OnConsole(wxCommandEvent& event);

	// Views Menu
	void OnViewMemoryToolbar(wxCommandEvent& event);

	// Tools Menu
	void OnOptions(wxCommandEvent& event);
	void OnLayoutSave(wxCommandEvent& event);
	void OnLayoutLoad(wxCommandEvent& event);

	/**
	 * If a modal dialog is used, it should be registered here. This is
	 * because we are running from a DLL, so the client can unload the DLL
	 * any time, even if a modal dialog is currently opended. This will lead
	 * to a crash on exit, so the modal dialog has to be closed on exit.
	 */
	void setModalDialog(wxDialog *dlg = nullptr) { m_modalDialog = dlg; }

protected:
	RegistersPanel *createRegistersPanel(void);
	MemoryPanel *createMemoryPanel(void);
	MemoryToolBar *getMemoryToolBar(void);
	DisasmPanel *createDisasmPanel(void);
	BreakpointPanel *createBreakpointPanel(void);
	ConsolePanel *createConsolePanel(void);

private:
	wxMenu *createFileMenu(void);
	wxMenu *createDebugMenu(void);
	wxMenu *createViewMenu(void);
	wxMenu *createToolsMenu(void);
	wxMenu *createHelpMenu(void);

	// TODO: Dummies can be removed later
	wxTextCtrl *CreateTextCtrl(const wxString &ctrl_text = wxEmptyString);
	wxTreeCtrl *CreateFileBrowser();

private:
	wxAuiManager m_manager;

	wxStatusBar *m_statusBar;
	wxMenuBar *m_frameMenu;
	DocumentPanel *m_documents;

	RegistersPanel *m_registerPanel;
	MemoryPanel *m_memoryPanel;
	MemoryToolBar *m_memoryToolBar;
	DisasmPanel *m_disasmPanel;
	BreakpointPanel *m_breakpointPanel;
	ConsolePanel *m_consolePanel;

	wxDialog *m_modalDialog;

	bool m_closeByMenu;

	wxDECLARE_EVENT_TABLE();
};
