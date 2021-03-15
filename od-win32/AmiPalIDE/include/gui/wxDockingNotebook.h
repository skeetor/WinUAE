#pragma once

#include <wx/aui/aui.h>

class wxAuiTabCtrlInfo
{
public:
	class PageInfo
	{
	public:
		PageInfo(wxWindow *page = nullptr, int index = -1, int pageIndex = -1)
		: m_page(page)
		, m_tabIndex(index)
		, m_pageIndex(pageIndex)
		{
		}

		wxWindow *m_page;
		int m_tabIndex;
		int m_pageIndex;
	};

public:
	wxAuiTabCtrlInfo()
	: m_tabCtrl(nullptr)
	, m_saveIndex(-1)
	, m_left(nullptr)
	, m_right(nullptr)
	, m_top(nullptr)
	, m_bottom(nullptr)
	{
	}

	wxAuiTabCtrlInfo(wxAuiTabCtrl *ctrl, wxWindow *page, int tabIndex, int pageIndex, uint32_t saveIndex, wxString const &name)
	: m_saveIndex(saveIndex)
	, m_left(nullptr)
	, m_right(nullptr)
	, m_top(nullptr)
	, m_bottom(nullptr)
	, m_name(name)		// Just for debugging
	{
		m_tabCtrl = ctrl;

		// When restoring, this may be a nullptr
		if(page)
			m_position = page->GetPosition();

		addPage(page, tabIndex, pageIndex);
	}

	operator wxAuiTabCtrl *() { return m_tabCtrl; }
	operator wxAuiTabCtrl const *() const { return m_tabCtrl; }

	void addPage(wxWindow *page, int tabIndex, int pageIndex)
	{
		PageInfo pi(page, tabIndex, pageIndex);
		m_pages.push_back(pi);
	}

	wxAuiTabCtrl *m_tabCtrl;
	uint32_t m_saveIndex;
	std::vector<PageInfo> m_pages;
	wxPoint m_position;
	wxSize m_size;
	wxString m_name;	// Only for debugging
	wxAuiTabCtrlInfo *m_left;
	wxAuiTabCtrlInfo *m_right;
	wxAuiTabCtrlInfo *m_top;
	wxAuiTabCtrlInfo *m_bottom;
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

	/**
	 * Move the page from the source tab ctrl to the destination. If this was the last
	 * page, the tab ctrl will be removed.
	 */
	void MovePage(wxAuiTabCtrl *src, int tabPageIndex, wxAuiTabCtrl *dest, bool select = true);

protected: // wxAuiNotebook internals

	/**
	 * Update the relations left/top/... in the list.
	 */
	void updateTabRelations(std::vector<wxAuiTabCtrlInfo> &tabInfos);

	/**
	 * Return the nearest tab in the specified direction. direction may only be
	 * one of wxLEFT, wxRIGHT, wxTOP or wxBOTTOM. They also may not be combined
	 * like wxLEFT|wxTOP.
	 */
	wxAuiTabCtrlInfo *nearestTab(wxAuiTabCtrlInfo const &ctrl, int direction, std::vector<wxAuiTabCtrlInfo> &tabInfos);

	std::vector<wxAuiTabCtrlInfo> getTabControls(void);

	/**
	 * Parse the layout from a string created by SavePerspective().
	 */
	bool parseTabControls(wxString &layout, std::vector<wxAuiTabCtrlInfo> &infos);
};
