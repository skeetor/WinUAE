#include "gui/wxDockingNotebook.h"
#include "utils/StringUtils.h"

#include <wx/confbase.h>

#include <string>

using namespace std;

#define PARSE_RELATION(direction) \
	n = -2;\
	readToken(layout, wxSTRINGIZE(direction), n);\
	if (n == -2)\
		return false;\
	if (n != -1)\
		info.m_##direction = n

bool readToken(wxString & s, wxString const &name, wxString & value, bool allowEmptyValue = false)
{
	value = getToken(s, "|");
	if (value.empty())
		return false;

	wxString token = getToken(value, "=");
	if (token != name || (!allowEmptyValue && value.empty()))
		return false;

	return true;
}

bool readToken(wxString & s, wxString const &name, unsigned long &value, unsigned long defaultValue = -1)
{
	wxString v;

	if (!readToken(s, name, v, true))
		return false;

	if (!v.ToCULong(&value))
	{
		value = defaultValue;
		return false;
	}

	return true;
}

wxDockingNotebook::wxDockingNotebook(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
: wxAuiNotebook(parent, id, pos, size, style)
{
}

wxDockingNotebook::~wxDockingNotebook()
{
}

wxAuiManager *wxDockingNotebook::GetManager()
{
	return &m_mgr;
}

// Group all pages into their tabcontrols. This will not be
// neccessary when we move the code to wxAuiBook, as this information
// is already there in wxTabFrame which is not accessible outside.
vector<wxAuiTabCtrlInfo> wxDockingNotebook::getTabControls(void)
{
	vector<wxAuiTabCtrlInfo> infos;

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
				tci.addPage(tabIndex, i);
				ctrl = nullptr;
				break;
			}
		}

		if (ctrl)
			infos.push_back(wxAuiTabCtrlInfo(ctrl, page, tabIndex, infos.size(), infos.size(), GetPageText(i)));
	}

	return infos;
}

void wxDockingNotebook::updateTabRelations(vector<wxAuiTabCtrlInfo> &infos)
{
	for (wxAuiTabCtrlInfo &info : infos)
	{
		info.m_left = nearestTab(info, wxLEFT, infos);
		info.m_right = nearestTab(info, wxRIGHT, infos);
		info.m_top = nearestTab(info, wxTOP, infos);
		info.m_bottom = nearestTab(info, wxBOTTOM, infos);
	}
}

int32_t wxDockingNotebook::nearestTab(wxAuiTabCtrlInfo const &ctrl, int direction, std::vector<wxAuiTabCtrlInfo> &infos)
{
	wxPoint const &ctrlPos = ctrl.m_position;
	wxAuiTabCtrlInfo *nearest = nullptr;
	int32_t rc = -1;

	for (int32_t i = 0; i < infos.size(); i++)
	{
		wxAuiTabCtrlInfo &info = infos[i];

		if (info.m_tabCtrl == ctrl.m_tabCtrl)
			continue;

		wxPoint const &pi = info.m_position;
		switch (direction)
		{
			case wxLEFT:
			{
				if (ctrlPos.y != pi.y)
					continue;

				if (ctrlPos.x > pi.x)
				{
					if (!nearest)
					{
						nearest = &info;
						rc = i;
						continue;
					}

					if (pi.x > nearest->m_position.x)
					{
						nearest = &info;
						rc = i;
						continue;
					}
				}
			}
			break;

			case wxRIGHT:
			{
				if (ctrlPos.y != pi.y)
					continue;

				if (ctrlPos.x < pi.x)
				{
					if (!nearest)
					{
						nearest = &info;
						rc = i;
						continue;
					}

					if (pi.x < nearest->m_position.x)
					{
						nearest = &info;
						rc = i;
						continue;
					}
				}
			}
			break;

			case wxTOP:
			{
				if (ctrlPos.x != pi.x)
					continue;

				if (ctrlPos.y < pi.y)
				{
					if (!nearest)
					{
						nearest = &info;
						rc = i;
						continue;
					}

					if (pi.y < nearest->m_position.y)
					{
						nearest = &info;
						rc = i;
						continue;
					}
				}
			}
			break;

			case wxBOTTOM:
			{
				if (ctrlPos.x != pi.x)
					continue;

				if (ctrlPos.y > pi.y)
				{
					if (!nearest)
					{
						nearest = &info;
						rc = i;
						continue;
					}

					if (pi.y > nearest->m_position.y)
					{
						nearest = &info;
						rc = i;
						continue;
					}
				}
			}
			break;
		}
	}

	return rc;
}

void wxDockingNotebook::MovePage(wxAuiTabCtrl *src, int tabPageIndex, wxAuiTabCtrl *dest, bool select)
{
	// remove the page from the source tabs
	wxAuiNotebookPage pageInfo = src->GetPage(tabPageIndex);
	pageInfo.active = false;
	src->RemovePage(pageInfo.window);
	if (src->GetPageCount() > 0)
	{
		src->SetActivePage((size_t)0);
		src->DoShowHide();
		src->Refresh();
	}

	// add the page to the destination tabs
	dest->InsertPage(pageInfo.window, pageInfo, 0);
	if (src->GetPageCount() == 0)
	{
		RemoveEmptyTabFrames();
	}

	DoSizing();
	dest->DoShowHide();
	dest->Refresh();

	if (select)
	{
		// force the set selection function to reset the selection
		m_curPage = -1;

		// set the active page to the one we just split off
		SetSelectionToPage(pageInfo);
	}

	UpdateHintWindowSize();
}

wxString wxDockingNotebook::SavePerspective(void)
{
	vector<wxAuiTabCtrlInfo> infos = getTabControls();
	wxString perspective = "views=" + to_string(infos.size());
	updateTabRelations(infos);

	int32_t ctrlId = 0;
	for (wxAuiTabCtrlInfo const &info : infos)
	{
		perspective += "|tabctrl=" + to_string(ctrlId++);
		wxSize sz = info.m_tabCtrl->GetSize();
		perspective += "|sz=" + to_string(sz.x) + ":" + to_string(sz.y);
		perspective += "|pagecnt=" + to_string(info.m_pages.size());

		for (wxAuiTabCtrlInfo::PageInfo const &page : info.m_pages)
		{
			perspective += "|tabindex=" + to_string(page.m_tabIndex);
			perspective += "|pageindex=" + to_string(page.m_pageIndex);
		}
		perspective += "|left=" + to_string(info.m_left);
		perspective += "|right=" + to_string(info.m_right);
		perspective += "|top=" + to_string(info.m_top);
		perspective += "|bottom=" + to_string(info.m_bottom);
	}

	return perspective;
}

bool wxDockingNotebook::parseTabControls(wxString &layout, vector<wxAuiTabCtrlInfo> &outInfos)
{
	outInfos.clear();

	wxString value;
	unsigned long n;

	if (!readToken(layout, "views", n))
		return false;

	vector<wxAuiTabCtrlInfo> infos(n);

	uint32_t i = 0;
	for (wxAuiTabCtrlInfo &info : infos)
	{
		if (!readToken(layout, "tabctrl", n))
			return false;

		if (!readToken(layout, "sz", value))
			return false;

		wxString s = getToken(value, ":");
		if (s.empty() || value.empty())
			return false;

		unsigned long w, h;
		if (!s.ToCULong(&w) || !value.ToCULong(&h))
			return false;
		info.m_size = wxSize(w, h);

		if (!readToken(layout, "pagecnt", n))
			return false;
		info.m_pages.resize(n);
		for (wxAuiTabCtrlInfo::PageInfo &page : info.m_pages)
		{
			if (!readToken(layout, "tabindex", n))
				return false;
			page.m_tabIndex = n;

			if (!readToken(layout, "pageindex", n))
				return false;
			page.m_pageIndex = n;
		}

		PARSE_RELATION(left);
		PARSE_RELATION(right);
		PARSE_RELATION(top);
		PARSE_RELATION(bottom);
	}

	outInfos = move(infos);

	return true;
}

bool wxDockingNotebook::LoadPerspective(wxString layout, bool update)
{
	// If the current notebook doesn't have enough pages, there is nothing
	// we can do, so we don't need to proceed. The LoadPerspective() only
	// reconstructs the layout, but it can not create any pages.
	// We could create empty dummy pages, so that the layout will always
	// be created, but should we do this? Another solution might be that
	// the user can pass a producer function which will create the required
	// pages if they are missing.
	size_t pages = GetPageCount();
	if (pages <= 1)
		return true;

	wxWindow *page = GetPage(0);
	wxAuiTabCtrl *root;
	int tabIndex;

	// Hmmmm.... There should be one page...
	if (!FindTab(page, &root, &tabIndex))
		return false;

	vector<wxAuiTabCtrlInfo> infos;
	if (!parseTabControls(layout, infos))
		return false;

	// First we need to move all pages to the same ctrl.
	for (size_t i = 1; i < pages; i++)
	{
		wxAuiTabCtrl *src;

		page = GetPage(i);
		if (!FindTab(page, &src, &tabIndex))
			return false;

		MovePage(src, tabIndex, root, false);
	}

	for (wxAuiTabCtrlInfo &info : infos)
	{
		if (info.m_left != -1)
		{
		}
	}

	return true;
}
