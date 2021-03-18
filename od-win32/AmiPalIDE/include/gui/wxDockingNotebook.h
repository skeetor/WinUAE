#pragma once

#include <wx/aui/aui.h>
#include <set>

class wxAuiPageCtrlMapping
{
public:
	wxAuiPageCtrlMapping(int tabIndex = -1, int pageIndex = -1)
		: m_tabIndex(tabIndex)
		, m_pageIndex(pageIndex)
	{
	}

	int m_tabIndex;
	int m_pageIndex;
};

class wxAuiLayoutInfo
{
public:
	wxAuiLayoutInfo()
	: m_tabCtrl(nullptr)
	, m_left(-1)
	, m_right(-1)
	, m_top(-1)
	, m_bottom(-1)
	{
	}

	wxAuiLayoutInfo(wxAuiTabCtrl *ctrl, wxWindow *page, int tabIndex, int pageIndex)
	: m_tabCtrl(ctrl)
	, m_left(-1)
	, m_right(-1)
	, m_top(-1)
	, m_bottom(-1)
	{
		// When restoring, this may be a nullptr
		if(page)
			m_position = page->GetPosition();

		addPage(tabIndex, pageIndex);
	}

	operator wxAuiTabCtrl *() { return m_tabCtrl; }
	operator wxAuiTabCtrl const *() const { return m_tabCtrl; }

	void addPage(int tabIndex, int pageIndex)
	{
		wxAuiPageCtrlMapping pm(tabIndex, pageIndex);
		m_pages.push_back(pm);
	}

	size_t tabPage(int pageIndex)
	{
		for (wxAuiPageCtrlMapping const &pm : m_pages)
		{
			if (pm.m_pageIndex == pageIndex)
				return pm.m_tabIndex;
		}

		return -1;
	}

	wxAuiTabCtrl *m_tabCtrl;
	std::vector<wxAuiPageCtrlMapping> m_pages;
	wxPoint m_position;
	wxSize m_size;

	// Index of the TabCtrlInfo
	int32_t m_left;
	int32_t m_right;
	int32_t m_top;
	int32_t m_bottom;
};

class wxDockingNotebook
: public wxAuiNotebook
{
public:
	wxDockingNotebook(wxWindow *parent,
		wxWindowID id = wxID_ANY,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = wxAUI_NB_DEFAULT_STYLE);
	~wxDockingNotebook() override;

public:
	wxString SerializeLayout(void);
	bool DeserializeLayout(wxString layout, bool update = true);

	wxAuiManager *GetManager();

protected: // wxAuiNotebook internals
	/**
	 * Move the page from the source tab ctrl to the destination. If this was the last
	 * page, the tab ctrl will be removed. If src == dest, false is returned.
	 */
	void MovePage(wxAuiTabCtrl *src, int tabPageIndex, wxAuiTabCtrl *dest, int destTabIndex = -1, bool select = true);

	/**
	 * Update the relations left/top/... in the list.
	 */
	void updateTabRelations(std::vector<wxAuiLayoutInfo> &tabInfos);

	/**
	 * Return the nearest tab in the specified direction. direction may only be
	 * one of wxLEFT, wxRIGHT, wxTOP or wxBOTTOM. They also may not be combined
	 * like wxLEFT|wxTOP.
	 */
	int32_t nearestTab(wxAuiLayoutInfo const &ctrl, int direction, std::vector<wxAuiLayoutInfo> &tabInfos);

	std::vector<wxAuiLayoutInfo> getTabControls(void);

	/**
	 * Parse the layout from a string created by SerializeLayout().
	 */
	bool parseTabControls(wxString &layout, std::vector<wxAuiLayoutInfo> &infos);

	/**
	 * Creates a new tabcontrol in the specified direction. If the direction already has a
	 * tabcontrol, then nothing happens. The control to be splitted must have at least two
	 * pages, otherwise it couldn't split. In that case it assumes that the first entry
	 * has enough pages, and moves the last page to the splitter, so it can be splitted.
	 * If that also fails, false is returned.
	 */
	bool RestoreSplit(std::vector<wxAuiLayoutInfo> &infos, std::set<int> &unassignedPages, wxAuiLayoutInfo &splitter, int32_t targetIndex, int direction);
};
