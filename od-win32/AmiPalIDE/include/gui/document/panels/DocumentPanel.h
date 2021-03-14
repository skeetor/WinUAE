#pragma once

#include <wx/aui/aui.h>

#include "gui/document/Document.h"

class wxAuiTabCtrlInfo;

class DocumentPanel
: public wxAuiNotebook
, public DocumentWindow
{
public:
	DocumentPanel(wxWindow *parent, wxWindowID id = wxID_ANY);
	~DocumentPanel() override;

public:
	bool AddPage(Document *page, const wxString &caption, bool select = false, wxAuiPaneInfo *paneInfo = nullptr, const wxBitmap &bitmap = wxNullBitmap);
	bool InsertPage(size_t pageIdx, Document *page, const wxString &caption, bool select = false, wxAuiPaneInfo *p = nullptr, const wxBitmap &bitmap = wxNullBitmap);

	wxWindow *getWindow(void) override { return this; }

	bool serialize(wxString groupId, wxConfigBase *config) override;
	bool deserialize(wxString groupId, wxConfigBase *config) override;

	wxString SavePerspective(void);
	bool LoadPerspective(wxString layout, bool update = true);

	wxAuiManager *GetManager();

protected:
	virtual void OnPageChanged(wxAuiNotebookEvent &event);
	virtual void OnPageClosing(wxAuiNotebookEvent &event);

protected: // wxAuiNotebook internals
	/**
	 * Get the direction for the tab in relation to the reference tabctrl.
	 * Return value should actually be a wxDirection enum, but it lacks wxCENTER. :(
	 */
	int getDirection(wxAuiTabCtrlInfo &refCtrl, wxAuiTabCtrlInfo &tabCtrl);

private:
	wxDECLARE_EVENT_TABLE();
};
