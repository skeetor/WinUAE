#pragma once

#include <wx/aui/aui.h>

#include "gui/document/Document.h"

///////////////////////////////////////////////////////////////////////////
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

	bool serialize(wxString const &groupId, wxConfigBase *config) override;
	bool deserialize(wxString const &groupId, wxConfigBase *config) override;

	wxString SavePerspective(void);
	void LoadPerspective(wxString const &layout, bool update = true);

	wxAuiManager *GetManager();

protected:
	virtual void OnPageChanged(wxAuiNotebookEvent &event);
	virtual void OnPageClosing(wxAuiNotebookEvent &event);

private:
	wxDECLARE_EVENT_TABLE();
};
