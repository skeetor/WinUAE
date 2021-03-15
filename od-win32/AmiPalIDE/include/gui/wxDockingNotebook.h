#pragma once

#include <wx/aui/aui.h>

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
	, m_left(nullptr)
	, m_right(nullptr)
	, m_top(nullptr)
	, m_bottom(nullptr)
	{
	}

	wxAuiTabCtrlInfo(wxAuiTabCtrl *ctrl, wxWindow *page, int tabIndex, wxString const &name)
	: m_left(nullptr)
	, m_right(nullptr)
	, m_top(nullptr)
	, m_bottom(nullptr)
	, m_name(name)		// Just for debugging
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
	wxString m_name;
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

protected: // wxAuiNotebook internals
	/**
	 * Get the direction for the tab in relation to the reference tabctrl.
	 * Return value should actually be a wxDirection enum, but it lacks wxCENTER. :(
	 */
	int getDirection(wxAuiTabCtrlInfo &refCtrl, wxAuiTabCtrlInfo &tabCtrl);

	/**
	 * Update the relations left/top/... in the list.
	 */
	void updateTabRelations(std::vector<wxAuiTabCtrlInfo> &tabInfos);

	/**
	 * Return the nearest tab in the specified direction. direction may only be
	 * one of wxLEFT, wxRIGHT, wxTOP and wxBOTTOM. They also may not be combined
	 * like wxLEFT|wxTOP.
	 */
	wxAuiTabCtrlInfo *nearestTab(wxAuiTabCtrlInfo const &ctrl, int direction, std::vector<wxAuiTabCtrlInfo> &tabInfos);

	std::vector<wxAuiTabCtrlInfo> getTabControls(void);
};
