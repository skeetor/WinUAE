#include "gui/document/panels/DocumentPanel.h"
#include "utils/StringUtils.h"

#include <wx/confbase.h>

#include <string>

using namespace std;

class wxAuiTabCtrlInfo
{
public:
	class PageInfo
	{
	public:
		PageInfo(wxWindow *page = nullptr, int index = -1)
			: m_page(page)
			, m_tabIndex(index)
		{
		}

		wxWindow *m_page;
		int m_tabIndex;
	};

public:
	wxAuiTabCtrlInfo()
		: m_tabCtrl(nullptr)
	{
	}

	wxAuiTabCtrlInfo(wxAuiTabCtrl *ctrl, wxWindow *page, int tabIndex)
	{
		m_tabCtrl = ctrl;
		m_position = page->GetPosition();
		addPage(page, tabIndex);
	}

	operator wxAuiTabCtrl *() { return m_tabCtrl; }
	operator wxAuiTabCtrl const *() const { return m_tabCtrl; }

	void addPage(wxWindow *page, int tabIndex)
	{
		PageInfo pi(page, tabIndex);
		m_pages.push_back(pi);
	}

	wxAuiTabCtrl *m_tabCtrl;
	std::vector<PageInfo> m_pages;
	wxPoint m_position;
};

wxBEGIN_EVENT_TABLE(DocumentPanel, wxAuiNotebook)

	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, DocumentPanel::OnPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, DocumentPanel::OnPageClosing)

wxEND_EVENT_TABLE()

DocumentPanel::DocumentPanel(wxWindow *parent, wxWindowID id)
: wxAuiNotebook(parent, id, wxDefaultPosition, parent->FromDIP(wxSize(430, 200)),
	wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_MIDDLE_CLICK_CLOSE)
, DocumentWindow("DocumentPanel|" + toHexString(id), this)
{
}

DocumentPanel::~DocumentPanel()
{
}

wxAuiManager *DocumentPanel::GetManager()
{
	return &m_mgr;
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

class wxAuiTabCtrlRelation
{
public:
	wxAuiTabCtrlRelation()
	{
	}

	wxAuiTabCtrlInfo m_refCtrl;
	vector<wxAuiTabCtrlInfo> m_tabs;
};

int DocumentPanel::getDirection(wxAuiTabCtrlInfo &refCtrl, wxAuiTabCtrlInfo &tabCtrl)
{
	int rc = 0;

	if (refCtrl.m_position.x != tabCtrl.m_position.x)
	{
		if (refCtrl.m_position.x < tabCtrl.m_position.x)
			rc |= wxLEFT;
		else
			rc |= wxRIGHT;
	}

	if (refCtrl.m_position.y != tabCtrl.m_position.y)
	{
		if (refCtrl.m_position.y < tabCtrl.m_position.y)
			rc |= wxBOTTOM;
		else
			rc |= wxTOP;
	}

	return rc;
}

wxAuiTabCtrlRelation *findRelation()
{
	return nullptr;
}

wxString DocumentPanel::SavePerspective(void)
{
	wxString perspective = "pages=" + to_string(GetPageCount());
	vector<wxAuiTabCtrlInfo> infos;

	// First we group all pages into their tabcontrols. This will not be
	// neccessary when we move the code to wxAuiBook, as this information
	// is already there in wxTabFrame which is not accessible outside.
	for (size_t i = 0; i < GetPageCount(); i++)
	{
		wxWindow *page = GetPage(i);
		wxAuiTabCtrl *ctrl;
		int tabIndex;

		if (!FindTab(page, &ctrl, &tabIndex))
			continue;

		for (wxAuiTabCtrlInfo &tci : infos)
		{
			if (tci.m_tabCtrl == ctrl)
			{
				tci.addPage(page, tabIndex);
				ctrl = nullptr;
				break;
			}
		}

		if (ctrl)
			infos.push_back(wxAuiTabCtrlInfo(ctrl, page, tabIndex));
	}

	wxAuiTabCtrlRelation *curRelation = nullptr;
	vector<wxAuiTabCtrlRelation> relations;
/*	{
		if (curRelation)
		{
			int direction = getDirection(curRelation->m_refCtrl, info.m_tabCtrl);
			if (direction == 0)
			{
				curRelation->m_tabs.push_back(info);
				continue;
			}
			else
			{
			}
		}

		wxAuiTabCtrlRelation relation;
		relation.m_refCtrl = info;
		relations.push_back(relation);
		curRelation = &relations[relations.size() - 1];
	}*/

	return perspective;
}

bool DocumentPanel::LoadPerspective(wxString layout, bool update)
{
	wxString field = getToken(layout, "|");

	if (field.empty())
		return true;

	field = getToken(layout, "pages=");

	return true;
}

bool DocumentPanel::serialize(wxString groupId, wxConfigBase *config)
{
	//config->Write(groupId +"_Layout", SavePerspective());

	if (!DocumentWindow::serialize(groupId, config))
		return false;

	config->Write(groupId + "Layout", SavePerspective());

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

	// TODO: This doesn't work for a notebook and must wait on a fix from wxWidgets
	LoadPerspective(config->Read(groupId + "Layout", ""));

	Thaw();
	//m_mgr.Update();

	return true;
}
