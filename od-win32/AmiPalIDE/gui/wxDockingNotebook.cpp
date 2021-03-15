#include "gui/wxDockingNotebook.h"
#include "utils/StringUtils.h"

#include <wx/confbase.h>

#include <string>

using namespace std;

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
				tci.addPage(page, tabIndex, i);
				ctrl = nullptr;
				break;
			}
		}

		if (ctrl)
			infos.push_back(wxAuiTabCtrlInfo(ctrl, page, tabIndex, i, infos.size(), GetPageText(i)));
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

wxAuiTabCtrlInfo *wxDockingNotebook::nearestTab(wxAuiTabCtrlInfo const &ctrl, int direction, std::vector<wxAuiTabCtrlInfo> &infos)
{
	wxPoint const &ctrlPos = ctrl.m_position;
	wxAuiTabCtrlInfo *nearest = nullptr;

	for (wxAuiTabCtrlInfo &info : infos)
	{
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
						continue;
					}

					if (pi.x > nearest->m_position.x)
					{
						nearest = &info;
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
						continue;
					}

					if (pi.x < nearest->m_position.x)
					{
						nearest = &info;
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
						continue;
					}

					if (pi.y < nearest->m_position.y)
					{
						nearest = &info;
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
						continue;
					}

					if (pi.y > nearest->m_position.y)
					{
						nearest = &info;
						continue;
					}
				}
			}
			break;
		}
	}

	return nearest;
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
	wxString perspective = "pages=" + to_string(GetPageCount());
	vector<wxAuiTabCtrlInfo> infos = getTabControls();
	updateTabRelations(infos);

	for (wxAuiTabCtrlInfo const &info : infos)
	{
		perspective += "|tabctrl=" + to_string(info.m_saveIndex);
		wxSize sz = info.m_tabCtrl->GetSize();
		perspective += "|sz=" + to_string(sz.x) + ":" + to_string(sz.y);
		perspective += "|pagecnt=" + to_string(info.m_pages.size());

		for (wxAuiTabCtrlInfo::PageInfo const &page : info.m_pages)
		{
			perspective += "|tabindex=" + to_string(page.m_tabIndex);
			perspective += "|pageindex=" + to_string(page.m_pageIndex);
		}
		perspective += "|left=" + ((info.m_left) ? to_string(info.m_left->m_saveIndex) : "");
		perspective += "|right=" + ((info.m_right) ? to_string(info.m_right->m_saveIndex) : "");
		perspective += "|top=" + ((info.m_top) ? to_string(info.m_top->m_saveIndex) : "");
		perspective += "|bottom=" + ((info.m_bottom) ? to_string(info.m_bottom->m_saveIndex) : "");
	}

	return perspective;
}

bool readToken(wxString &s, wxString const &name, wxString &value, bool checkValueEmpty = true)
{
	value = getToken(s, "|");
	if (value.empty())
		return false;

	wxString token = getToken(value, "=");
	if (token != name || (checkValueEmpty && value.empty()))
		return false;

	return true;
}

bool readToken(wxString &s, wxString const &name, unsigned long &value, unsigned long defaultValue = -1)
{
	wxString v;

	if (!readToken(s, name, v))
		return false;

	if (!v.ToCULong(&value))
	{
		value = defaultValue;
		return false;
	}

	return true;
}

bool wxDockingNotebook::parseTabControls(wxString &layout, vector<wxAuiTabCtrlInfo> &outInfos)
{
	outInfos.clear();

	wxString value;
	unsigned long n;

	if (!readToken(layout, "pages", n))
		return false;

	vector<wxAuiTabCtrlInfo> infos(n);

	for (wxAuiTabCtrlInfo &info : infos)
	{
		if (!readToken(layout, "tabctrl", n))
			return false;
		info.m_saveIndex = n;

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

		// -2 can never be a valid value, so we know that the function failed if it is
		// still -2. In case of an empty value, it will be set to -1.
		n = -2;
		readToken(layout, "left", n);
		if (n == -2 )
			return false;
		/*
		perspective += "|left=" + ((info.m_left) ? to_string(info.m_left->m_saveIndex) : "");
		perspective += "|right=" + ((info.m_right) ? to_string(info.m_right->m_saveIndex) : "");
		perspective += "|top=" + ((info.m_top) ? to_string(info.m_top->m_saveIndex) : "");
		perspective += "|bottom=" + ((info.m_bottom) ? to_string(info.m_bottom->m_saveIndex) : "");
		*/
	}

	outInfos = move(infos);

	return true;
}

bool wxDockingNotebook::LoadPerspective(wxString layout, bool update)
{
	vector<wxAuiTabCtrlInfo> infos;
	if (!parseTabControls(layout, infos))
		return false;

	return true;
}
