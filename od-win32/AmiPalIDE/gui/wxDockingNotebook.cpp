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
vector<wxAuiLayoutInfo> wxDockingNotebook::getTabControls(void)
{
	vector<wxAuiLayoutInfo> infos;

	for (size_t i = 0; i < GetPageCount(); i++)
	{
		wxWindow *page = GetPage(i);
		wxAuiTabCtrl *ctrl;
		int tabIndex;

		if (!FindTab(page, &ctrl, &tabIndex))
			continue;

		for (wxAuiLayoutInfo &tci : infos)
		{
			if (tci.m_tabCtrl == ctrl)
			{
				tci.addPage(tabIndex, i);
				ctrl = nullptr;
				break;
			}
		}

		if (ctrl)
			infos.push_back(wxAuiLayoutInfo(ctrl, page, tabIndex, infos.size(), infos.size(), GetPageText(i)));
	}

	return infos;
}

void wxDockingNotebook::updateTabRelations(vector<wxAuiLayoutInfo> &infos)
{
	for (wxAuiLayoutInfo &info : infos)
	{
		info.m_left = nearestTab(info, wxLEFT, infos);
		info.m_right = nearestTab(info, wxRIGHT, infos);
		info.m_top = nearestTab(info, wxTOP, infos);
		info.m_bottom = nearestTab(info, wxBOTTOM, infos);
	}
}

int32_t wxDockingNotebook::nearestTab(wxAuiLayoutInfo const &ctrl, int direction, std::vector<wxAuiLayoutInfo> &infos)
{
	wxPoint const &ctrlPos = ctrl.m_position;
	wxAuiLayoutInfo *nearest = nullptr;
	int32_t rc = -1;

	for (int32_t i = 0; i < infos.size(); i++)
	{
		wxAuiLayoutInfo &info = infos[i];

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

bool wxDockingNotebook::MovePage(wxAuiTabCtrl *src, int tabPageIndex, wxAuiTabCtrl *dest, bool select)
{
	if (src == dest)
		return false;

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

	return true;
}

wxString wxDockingNotebook::SerializeLayout(void)
{
	vector<wxAuiLayoutInfo> infos = getTabControls();
	wxString perspective = "views=" + to_string(infos.size());
	updateTabRelations(infos);

	int32_t ctrlId = 0;
	for (wxAuiLayoutInfo const &info : infos)
	{
		perspective += "|tabctrl=" + to_string(ctrlId++);
		wxSize sz = info.m_tabCtrl->GetSize();
		perspective += "|sz=" + to_string(sz.x) + ":" + to_string(sz.y);
		perspective += "|pagecnt=" + to_string(info.m_pages.size());

		for (wxAuiLayoutInfo::PageCtrlMapping const &page : info.m_pages)
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

bool wxDockingNotebook::parseTabControls(wxString &layout, vector<wxAuiLayoutInfo> &outInfos)
{
	outInfos.clear();

	wxString value;
	unsigned long n;

	if (!readToken(layout, "views", n))
		return false;

	vector<wxAuiLayoutInfo> infos(n);

	uint32_t i = 0;
	for (wxAuiLayoutInfo &info : infos)
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
		for (wxAuiLayoutInfo::PageCtrlMapping &page : info.m_pages)
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

bool wxDockingNotebook::RestoreSplit(vector<wxAuiLayoutInfo> &infos, vector<wxAuiLayoutInfo::PageCtrlMapping> &pages, wxAuiLayoutInfo &info, int32_t targetIndex, int direction)
{
	// The first item has to exist already and can not be a split target.
	// If the targetindex is < 0 then it doesn't have such a target, which
	// is just as well.
	if (!targetIndex || targetIndex < 0)
		return true;

	if (targetIndex >= infos.size())
		return false;

	wxAuiLayoutInfo *target = &infos[targetIndex];

	size_t pageindex = -1;
	wxWindow *w = nullptr;

	// The target was already split, so we are done.
	if (target->m_tabCtrl != nullptr)
		return true;

	// Move all pages here that we want to split.


	// If the current control doesn't have enough pages, we try to move one
	// from root here, so we can split it.
	pageindex = info.m_tabCtrl->GetPageCount()-1;
	if (pageindex <= 0)
	{
		pageindex = root->m_tabCtrl->GetPageCount()-1;
		if (pageindex <= 0)
			return false;

		// If we split to the left, we only want one tab in the new control
		// so we have to take the first one, otherwise all tabs would end up
		// in the new tabctrl, and we don't know where the vanished to. We
		// need them to stay in root, remember?
		if (direction == wxLEFT)
			pageindex = 0;

		w = root->m_tabCtrl->GetPage(pageindex).window;
		if (!MovePage(root->m_tabCtrl, pageindex, info.m_tabCtrl))
			return false;
	}
	else
	{
		if (direction == wxLEFT)
			pageindex = 0;

		w = info.m_tabCtrl->GetPage(pageindex).window;
	}

	// Until now this was the tabindex, but for the split we need to know
	// the page index from the notebook itself.
	pageindex = GetPageIndex(w);

	Split(pageindex, direction);

	// After the split we need to update the tabctrl with the newly created one
	return FindTab(w, &target->m_tabCtrl, &targetIndex);
}

bool wxDockingNotebook::DeserializeLayout(wxString layout, bool update)
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

	vector<wxAuiLayoutInfo> infos;
	if (!parseTabControls(layout, infos))
		return false;

	infos[0].m_tabCtrl = root;
	vector<wxAuiLayoutInfo::PageCtrlMapping> pageMappings;

	// First we need to move all pages to the same ctrl.
	for (size_t i = 1; i < pages; i++)
	{
		wxAuiTabCtrl *src;

		page = GetPage(i);
		if (!FindTab(page, &src, &tabIndex))
			return false;

		if (!infos[0].ownsPage(i))
			pageMappings.emplace_back(wxAuiLayoutInfo::PageCtrlMapping(tabIndex, i));

		if (src == root)
			continue;

		MovePage(src, tabIndex, root, false);
	}

	// Now we build the layout
	for (wxAuiLayoutInfo &info : infos)
	{
		RestoreSplit(infos, pageMappings, info, info.m_left, wxLEFT);
		RestoreSplit(infos, pageMappings, info, info.m_right, wxRIGHT);
		RestoreSplit(infos, pageMappings, info, info.m_top, wxTOP);
		RestoreSplit(infos, pageMappings, info, info.m_bottom, wxBOTTOM);
	}

	// The layout is in the correct order, but we still have to shuffle
	// the pages to their correct position. This has to be done seperatly
	// afterwards, because when we create the layout, we have to ensure
	// the for each split there is at least one page available.
	// This can not be guaranteed if a page is surrounded by pages with
	// only one tab each.
	for (wxAuiLayoutInfo &info : infos)
	{

	}

	return true;
}
