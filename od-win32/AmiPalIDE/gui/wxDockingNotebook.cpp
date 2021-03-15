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


int wxDockingNotebook::getDirection(wxAuiTabCtrlInfo &refCtrl, wxAuiTabCtrlInfo &tabCtrl)
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
				tci.addPage(page, tabIndex);
				ctrl = nullptr;
				break;
			}
		}

		if (ctrl)
			infos.push_back(wxAuiTabCtrlInfo(ctrl, page, tabIndex, GetPageText(i)));
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

wxString wxDockingNotebook::SavePerspective(void)
{
	wxString perspective = "pages=" + to_string(GetPageCount());
	vector<wxAuiTabCtrlInfo> infos = getTabControls();
	updateTabRelations(infos);

	return perspective;
}

bool wxDockingNotebook::LoadPerspective(wxString layout, bool update)
{
	wxString field = getToken(layout, "|");

	if (field.empty())
		return true;

	field = getToken(layout, "pages=");

	return true;
}
