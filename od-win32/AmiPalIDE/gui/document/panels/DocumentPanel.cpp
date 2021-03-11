#include "gui/document/panels/DocumentPanel.h"
#include "utils/StringUtils.h"

#include <wx/confbase.h>

#include <string>

using namespace std;

wxBEGIN_EVENT_TABLE(DocumentPanel, wxAuiNotebook)

	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, DocumentPanel::OnPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, DocumentPanel::OnPageClosing)

wxEND_EVENT_TABLE()

DocumentPanel::DocumentPanel(wxWindow *parent, wxWindowID id)
: wxAuiNotebook(parent, id, wxDefaultPosition, parent->FromDIP(wxSize(430, 200)), wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_MIDDLE_CLICK_CLOSE)
, DocumentWindow("DocumentPanel|" + toHexString(id), this)
{
}

DocumentPanel::~DocumentPanel()
{
}

bool DocumentPanel::AddPage(Document *page, const wxString &caption, bool select, const wxBitmap &bitmap)
{
	return wxAuiNotebook::AddPage(page->getWindow(), caption, select, bitmap);
}

bool DocumentPanel::DocumentPanel::InsertPage(size_t pageIdx, Document *page, const wxString &caption, bool select, const wxBitmap &bitmap)
{
	return wxAuiNotebook::InsertPage(pageIdx, page->getWindow(), caption, select, bitmap);
}

void DocumentPanel::OnPageChanged(wxAuiNotebookEvent &event)
{
	int page;

	if ((page = event.GetOldSelection()) != -1)
	{
		wxWindow *w = GetPage(page);
		Document *d = static_cast<Document *>(w->GetClientData());
		d->deactivate();
	}

	if ((page = event.GetSelection()) != -1)
	{
		wxWindow *w = GetPage(page);
		Document *d = reinterpret_cast<Document *>(w->GetClientData());
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
		Document *d = static_cast<Document *>(w->GetClientData());
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
			Document *d = static_cast<Document *>(w->GetClientData());
			d->activate();
		}
	}
}

bool DocumentPanel::serialize(wxString const &groupId, wxConfigBase *config)
{
	for (size_t i = 0; i < GetPageCount(); i++)
	{
		wxString id = groupId + "Panel_" + to_string(i);
		wxWindow *w = GetPage(i);
		Document *d = static_cast<Document *>(w->GetClientData());

		config->Write(id +"_Type", d->getTypeInfo());
		config->Write(id +"_Title", GetPageText(i));
		d->serialize(id + "_", config);
	}

	return true;
}

bool DocumentPanel::deserialize(wxString const &groupId, wxConfigBase *config)
{
	size_t i = 0;
	wxString v;
	wxString id = groupId + "Panel_" + to_string(i);

	while ((v = config->Read(id + "_Type", "")) != "")
	{
		Document *d = Document::createFromInfo(this, v);
		wxWindow *w = d->getWindow();

		checkException(!d, "Unknown type: ", id + "_Type", v);
		checkException((v = config->Read(id + "_Title", "")).empty(), "", id + "_Title", v);

		d->deserialize(id + "_", config);
		AddPage(d, v, false);

		i++;
		id = groupId + "Panel_" + to_string(i);
	}

	return true;
}
