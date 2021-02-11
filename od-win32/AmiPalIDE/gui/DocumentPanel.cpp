#include "gui/DocumentPanel.h"

wxBEGIN_EVENT_TABLE(DocumentPanel, wxAuiNotebook)

	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, DocumentPanel::OnPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, DocumentPanel::OnPageClosing)

wxEND_EVENT_TABLE()

DocumentPanel::DocumentPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
: wxAuiNotebook(parent, id, pos, size, style)
{
}

DocumentPanel::~DocumentPanel()
{
}

bool DocumentPanel::AddPage(DocumentView *page, const wxString &caption, bool select, const wxBitmap &bitmap)
{
	wxWindow *w = page->getWindow();
	w->SetClientData(page);
	return wxAuiNotebook::AddPage(w, caption, select, bitmap);
}

bool DocumentPanel::DocumentPanel::InsertPage(size_t pageIdx, DocumentView *page, const wxString &caption, bool select, const wxBitmap &bitmap)
{
	wxWindow *w = page->getWindow();
	w->SetClientData(page);

	return wxAuiNotebook::InsertPage(pageIdx, w, caption, select, bitmap);
}

void DocumentPanel::OnPageChanged(wxAuiNotebookEvent &event)
{
	int page;

	if ((page = event.GetOldSelection()) != -1)
	{
		wxWindow *w = GetPage(page);
		DocumentView *d = static_cast<DocumentView *>(w->GetClientData());
		d->deactivate();
	}

	if ((page = event.GetSelection()) != -1)
	{
		wxWindow *w = GetPage(page);
		DocumentView *d = reinterpret_cast<DocumentView *>(w->GetClientData());
		d->activate();
	}

	return;
}

void DocumentPanel::OnPageClosing(wxAuiNotebookEvent &event)
{
	int selected = GetSelection();
	int page = event.GetSelection();
	wxWindow *w;
	bool active = page == selected;

	if ((page = event.GetSelection()) != -1)
	{
		w = GetPage(page);
		DocumentView *d = static_cast<DocumentView *>(w->GetClientData());
		if (!d->closing(false, active))
		{
			event.Veto();
			return;
		}
	}

	if (active)
	{
		int cnt = GetPageCount() - 1;
		if (cnt <= 0)	// No more pages
			return;

		if (page != cnt)
			page++;			// Activate the next page
		else
			page--;			// Last page will active the previous

		w = GetPage(page);
		if (w)
		{
			DocumentView *d = static_cast<DocumentView *>(w->GetClientData());
			d->activate();
		}
	}
}
