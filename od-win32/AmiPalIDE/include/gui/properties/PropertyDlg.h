#pragma once

#include <vector>

#include <wx/srchctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

#include "gui/properties/PropertyPage.h"
#include "patterns/observer.h"

#define IDC_PROPERTY_SECTION_FILTER 1000
#define IDC_PROPERTY_SECTIONS 1001
#define IDC_PROPERTY_PAGES 1002
#define IDC_PROPERTY_OK 1003
#define IDC_PROPERTY_RESET 1004
#define IDC_PROPERTY_APPLY 1005

typedef wxTreeCtrl PropertyTree;
typedef wxSearchCtrl SectionFilter;
typedef wxNotebook PropertyPages;

class PropertyDlg
: public wxDialog
, Listener<PropertyPagePanel>
{
public:
	PropertyDlg(wxWindow *parent, const wxString &title = wxT("Properties"), const wxString &rootTitle = wxT("root"), wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(830, 468));
	~PropertyDlg() override;

	int ShowModal(void) override;

protected:
	virtual void OnInitDialog(wxInitDialogEvent &event);
	virtual void OnClose(wxCloseEvent &event);
	virtual void OnOK(wxCommandEvent &event);
	virtual void OnApply(wxCommandEvent &event);
	virtual void OnReset(wxCommandEvent &event);

	virtual void OnNodeSelection(wxTreeEvent &event);
	virtual void OnNodeSelectionChanging(wxTreeEvent &event);
	virtual void OnNodeTooltip(wxTreeEvent &event);

	virtual void OnSearch(wxCommandEvent &event);

	virtual void OnPageChanged(wxBookCtrlEvent &event);
	virtual void OnPageChanging(wxBookCtrlEvent &event);

	wxTreeItemId getRootId(void) { return m_rootId; }
	PropertyTree *getPropertyTree(void) { return m_propertyTree; }
	PropertyPages *getPropertyPages(void) { return m_propertyPages; }

	wxTreeItemId registerPropertyPage(wxTreeItemId id, const wxString &nodeName, PropertyPagePanel *page);

	SectionFilter *getSectionFilter(void) { return m_sectionFilter; }

	void handleNotification(PropertyPagePanel &page) override;

private:
	void Init();
	wxString getPageTitle(const PropertyPagePanel &page) const;

private:
	SectionFilter *m_sectionFilter;
	PropertyTree *m_propertyTree;
	PropertyPages *m_propertyPages;
	wxTreeItemId m_rootId;

	std::vector<PropertyPage *> m_pages;

	wxDECLARE_EVENT_TABLE();
};
