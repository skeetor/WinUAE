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
: wxDockingNotebook(parent, id, wxDefaultPosition, parent->FromDIP(wxSize(430, 200)),
	wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_MIDDLE_CLICK_CLOSE)
, DocumentWindow("DocumentPanel|" + toHexString(id), this)
{
}

DocumentPanel::~DocumentPanel()
{
}

bool DocumentPanel::AddPage(Document *page, const wxString &caption, bool select, wxAuiPaneInfo *paneInfo, const wxBitmap &bitmap)
{
	bool rc = wxAuiNotebook::AddPage(page->getWindow(), caption, select, bitmap);
/*	if (rc && paneInfo)
	{
		wxWindow *w = page->getWindow();
		wxAuiPaneInfo p = m_mgr.GetPane(w);
	}*/

	return rc;
}

bool DocumentPanel::DocumentPanel::InsertPage(size_t pageIdx, Document *page, const wxString &caption, bool select, wxAuiPaneInfo *p, const wxBitmap &bitmap)
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

bool DocumentPanel::serialize(wxString groupId, wxConfigBase *config)
{
	//config->Write(groupId +"_Layout", SavePerspective());

	if (!DocumentWindow::serialize(groupId, config))
		return false;

	config->Write(groupId + "Layout", SerializeLayout());

	for (size_t i = 0; i < GetPageCount(); i++)
	{
		wxString id = groupId + "Tab_" + to_string(i) + "_";
		wxWindow *w = GetPage(i);
		Document *d = static_cast<Document *>(w->GetClientData());

		config->Write(id +"Type", d->getTypeInfo());

		wxAuiPaneInfo info = m_mgr.GetPane(w);
		config->Write(id + "PaneInfo", m_mgr.SavePaneInfo(info));
		wxPoint p = w->GetPosition();
		wxString s = to_string(p.x) + ":" + to_string(p.y);
		config->Write(id + "Position", s);

		d->serialize(id , config);
	}

	return true;
}

bool DocumentPanel::deserialize(wxString groupId, wxConfigBase *config)
{
	size_t i = 0;
	wxString v;
	wxString id;

	if (!DocumentWindow::deserialize(groupId, config))
		return false;

	Freeze();

	while ((v = config->Read((id = groupId + "Tab_" + to_string(i++) + "_") + "Type", "")) != "")
	{
		Document *d = Document::createFromInfo(this, v);
		wxWindow *w = d->getWindow();

		checkException(!d, "Unknown type: ", id + "Type", v);

		d->deserialize(id, config);
		AddPage(d, d->getTitle(), false);
	}

	DeserializeLayout(config->Read(groupId + "Layout", ""));

	Thaw();

	return true;
}
