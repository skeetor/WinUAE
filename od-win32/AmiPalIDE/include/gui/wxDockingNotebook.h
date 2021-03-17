#pragma once

#include <wx/aui/aui.h>

class wxAuiTabCtrlInfo
{
public:
	class PageInfo
	{
	public:
		PageInfo(int tabIndex = -1, int pageIndex = -1)
		: m_tabIndex(tabIndex)
		, m_pageIndex(pageIndex)
		{
		}

		int m_tabIndex;
		int m_pageIndex;
	};

public:
	wxAuiTabCtrlInfo()
	: m_tabCtrl(nullptr)
	, m_left(-1)
	, m_right(-1)
	, m_top(-1)
	, m_bottom(-1)
	{
	}

	wxAuiTabCtrlInfo(wxAuiTabCtrl *ctrl, wxWindow *page, int tabIndex, int pageIndex, uint32_t saveIndex, wxString const &name)
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
		PageInfo pi(tabIndex, pageIndex);
		m_pages.push_back(pi);
	}

	wxAuiTabCtrl *m_tabCtrl;
	std::vector<PageInfo> m_pages;
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
	wxString SavePerspective(void);
	bool LoadPerspective(wxString layout, bool update = true);

	wxAuiManager *GetManager();

protected: // wxAuiNotebook internals
	/**
	 * Move the page from the source tab ctrl to the destination. If this was the last
	 * page, the tab ctrl will be removed. If src == dest, false is returned.
	 */
	bool MovePage(wxAuiTabCtrl *src, int tabPageIndex, wxAuiTabCtrl *dest, bool select = true);

	/**
	 * Update the relations left/top/... in the list.
	 */
	void updateTabRelations(std::vector<wxAuiTabCtrlInfo> &tabInfos);

	/**
	 * Return the nearest tab in the specified direction. direction may only be
	 * one of wxLEFT, wxRIGHT, wxTOP or wxBOTTOM. They also may not be combined
	 * like wxLEFT|wxTOP.
	 */
	int32_t nearestTab(wxAuiTabCtrlInfo const &ctrl, int direction, std::vector<wxAuiTabCtrlInfo> &tabInfos);

	std::vector<wxAuiTabCtrlInfo> getTabControls(void);

	/**
	 * Parse the layout from a string created by SavePerspective().
	 */
	bool parseTabControls(wxString &layout, std::vector<wxAuiTabCtrlInfo> &infos);

	/**
	 * Creates a new tabcontrol in the specified direction. If the direction already has a
	 * tabcontrol, then nothing happens. The control to be splitted must have at least two
	 * pages, otherwise it couldn't split. In that case it assumes that the first entry
	 * has enough pages, and moves the last page to the splitter, so it can be splitted.
	 * If that also fails, false is returned.
	 */
	bool RestoreSplit(std::vector<wxAuiTabCtrlInfo> &infos, wxAuiTabCtrlInfo &splitter, int32_t targetIndex, int direction);
};
