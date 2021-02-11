#pragma once

#include <wx/aui/aui.h>
#include "gui/DocumentView.h"

///////////////////////////////////////////////////////////////////////////
class DocumentPanel
: public wxAuiNotebook
{
public:
	DocumentPanel(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE);
	~DocumentPanel() override;

public:
	bool AddPage(DocumentView *page, const wxString &caption, bool select = false, const wxBitmap &bitmap = wxNullBitmap);
	bool InsertPage(size_t pageIdx, DocumentView *page, const wxString &caption, bool select = false, const wxBitmap &bitmap = wxNullBitmap);

protected:
	virtual void OnPageChanged(wxAuiNotebookEvent &event);
	virtual void OnPageClosing(wxAuiNotebookEvent &event);

private:
	wxDECLARE_EVENT_TABLE();
};
