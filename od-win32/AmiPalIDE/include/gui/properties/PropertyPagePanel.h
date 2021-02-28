#pragma once

#include <wx/panel.h>

#include "algorithms/observer.h"

class PropertyDlg;

class PropertyPagePanel
: public wxPanel
, public Dispatcher<PropertyPagePanel>
{
	friend PropertyDlg;

public:
	PropertyPagePanel(wxWindow *parent, const wxString &title, wxWindowID id = wxID_ANY, const wxString &tooltip = wxEmptyString)
	: wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SUNKEN, wxASCII_STR(wxPanelNameStr))
	, m_title(title)
	, m_active(false)
	, m_dirty(false)
	{
	}

	~PropertyPagePanel(void) override {}

	void setTitle(const wxString &title) { m_title = title; }
	const wxString &getTitle(void ) const { return m_title; }

	void setTooltip(const wxString &tooltip) { m_tooltip = tooltip; }
	const wxString &getTooltip(void) const { return m_tooltip; }

public: // Property Page interface
	/**
	 * Called when a node in the treeitem is selected and the page is shown to the user 
	 */
	virtual void activate(void) {}

	/**
	 * Called when another node is selected and the current page is no longer shown.
	 */
	virtual void deactivate(void) {};

	/**
	 * Called when the applybutton is pressed. The page may not neccessarily be active.
	 */
	virtual void apply(void) = 0;

	/**
	 * Reset the setting the user currently changed, to their original values.
	 */
	virtual void reset(void) = 0;

protected:
	bool isActive(void) const { return m_active; }
	void setActive(bool active) { m_active = active; }

	bool isDirty(void) const { return m_dirty; }
	void setDirty(bool dirty)
	{
		m_dirty = dirty;
		notify();
	}

private:
	wxString m_title;
	wxString m_tooltip;

	bool m_active:1;
	bool m_dirty:1;
};
