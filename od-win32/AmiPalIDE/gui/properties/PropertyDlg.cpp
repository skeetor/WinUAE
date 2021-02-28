#include "gui/properties/PropertyDlg.h"
#include "gui/AmiPalIDE.h"
#include "gui/MainFrame.h"

#include <wx/gbsizer.h>
#include <wx/button.h>

wxBEGIN_EVENT_TABLE(PropertyDlg, wxDialog)

	EVT_CLOSE(PropertyDlg::OnClose)
	EVT_INIT_DIALOG(PropertyDlg::OnInitDialog)

	EVT_BUTTON(IDC_PROPERTY_OK, PropertyDlg::OnOK)
	EVT_BUTTON(IDC_PROPERTY_APPLY, PropertyDlg::OnApply)
	EVT_BUTTON(IDC_PROPERTY_RESET, PropertyDlg::OnReset)

	EVT_SEARCH(IDC_PROPERTY_SECTION_FILTER, PropertyDlg::OnSearch)

	EVT_TREE_SEL_CHANGED(IDC_PROPERTY_SECTIONS, PropertyDlg::OnNodeSelection)
	EVT_TREE_SEL_CHANGING(IDC_PROPERTY_SECTIONS, PropertyDlg::OnNodeSelectionChanging)
	EVT_TREE_ITEM_GETTOOLTIP(IDC_PROPERTY_SECTIONS, PropertyDlg::OnNodeTooltip)

	EVT_NOTEBOOK_PAGE_CHANGED(IDC_PROPERTY_PAGES, PropertyDlg::OnPageChanged)
	EVT_NOTEBOOK_PAGE_CHANGING(IDC_PROPERTY_PAGES, PropertyDlg::OnPageChanging)

wxEND_EVENT_TABLE()

PropertyDlg::PropertyDlg(wxWindow *parent, const wxString &title, const wxString &rootTitle, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxDialog(parent, id, title, pos, size, wxCAPTION | wxCLOSE_BOX | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	Init();
	m_rootId = m_propertyTree->AddRoot(rootTitle);
}

PropertyDlg::~PropertyDlg()
{
	// The PropertyPage object is deleted by the treecontrol itself.
	for (PropertyPage *p : m_pages)
	{
		p->getData()->removeListener(this);
		if (!p->isAttached())
			delete p->getData();
	}
}

void PropertyDlg::Init()
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxGridBagSizer *sizer;
	sizer = new wxGridBagSizer(0, 0);
	sizer->SetFlexibleDirection(wxBOTH);
	sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	m_sectionFilter = new SectionFilter(this, IDC_PROPERTY_SECTION_FILTER, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
#ifndef __WXMAC__
	m_sectionFilter->ShowSearchButton(true);
#endif
	m_sectionFilter->ShowCancelButton(false);
	sizer->Add(m_sectionFilter, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL, 5);

	m_propertyTree = new PropertyTree(this, IDC_PROPERTY_SECTIONS, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_SINGLE | wxTR_TWIST_BUTTONS | wxBORDER_SUNKEN);
	sizer->Add(m_propertyTree, wxGBPosition(1, 0), wxGBSpan(4, 1), wxALL | wxEXPAND, 5);

	m_propertyPages = new PropertyPages(this, IDC_PROPERTY_PAGES, wxDefaultPosition, wxDefaultSize, 0 | wxBORDER_SUNKEN);

	sizer->Add(m_propertyPages, wxGBPosition(0, 1), wxGBSpan(5, 1), wxALL | wxEXPAND, 5);

	wxButton *btn;
	btn = new wxButton(this, IDC_PROPERTY_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(btn, wxGBPosition(1, 2), wxGBSpan(1, 1), wxALIGN_BOTTOM | wxALL, 5);

	btn = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(btn, wxGBPosition(2, 2), wxGBSpan(1, 1), wxALL, 5);

	btn = new wxButton(this, IDC_PROPERTY_RESET, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(btn, wxGBPosition(3, 2), wxGBSpan(1, 1), wxALL, 5);

	btn = new wxButton(this, IDC_PROPERTY_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(btn, wxGBPosition(4, 2), wxGBSpan(1, 1), wxALL, 5);

	sizer->AddGrowableCol(1);
	sizer->AddGrowableRow(1);

	this->SetSizer(sizer);
	this->Layout();

	this->Centre(wxBOTH);
	m_propertyTree->SetFocus();
}

wxString PropertyDlg::getPageTitle(const PropertyPagePanel &page) const
{
	wxString title = page.getTitle();
	if (page.isDirty())
		title += " *";

	return title;
}

wxTreeItemId PropertyDlg::registerPropertyPage(wxTreeItemId id, const wxString &nodeName, PropertyPagePanel *page)
{
	PropertyPage *data = new PropertyPage(page);
	m_pages.push_back(data);
	page->addListener(this);
	page->Hide();
	return m_propertyTree->AppendItem(id, nodeName, -1, -1, data);
}

int PropertyDlg::ShowModal(void)
{
	wxGetApp().m_mainFrame->setModalDialog(this);
	int rc = wxDialog::ShowModal();
	wxGetApp().m_mainFrame->setModalDialog(nullptr);

	return rc;
}

void PropertyDlg::OnClose(wxCloseEvent &event)
{
	EndModal(wxID_CANCEL);
}

void PropertyDlg::OnInitDialog(wxInitDialogEvent &event)
{
}

void PropertyDlg::OnOK(wxCommandEvent &event)
{
	OnApply(event);
	EndModal(wxID_OK);
}

void PropertyDlg::OnApply(wxCommandEvent &event)
{
	PropertyPagePanel *page = static_cast<PropertyPagePanel *>(m_propertyPages->GetCurrentPage());
	page->apply();
	page->setDirty(false);
}

void PropertyDlg::OnReset(wxCommandEvent &event)
{
	PropertyPagePanel *page = static_cast<PropertyPagePanel *>(m_propertyPages->GetCurrentPage());
	page->reset();
	page->setDirty(false);
}

void PropertyDlg::OnNodeSelection(wxTreeEvent &event)
{
}

void PropertyDlg::OnNodeSelectionChanging(wxTreeEvent &event)
{
	wxTreeItemId id = event.GetOldItem();
	if (id.IsOk())
	{
		PropertyPage *pp = static_cast<PropertyPage *>(m_propertyTree->GetItemData(id));
		pp->setAttached(false);
		PropertyPagePanel *p = pp->getData();
		p->setActive(false);
		p->deactivate();
		m_propertyPages->RemovePage(0);
	}

	id = event.GetItem();
	if (id.IsOk())
	{
		PropertyPage *pp = static_cast<PropertyPage *>(m_propertyTree->GetItemData(id));
		pp->setAttached(true);
		PropertyPagePanel *p = pp->getData();
		p->setActive(true);
		p->activate();
		m_propertyPages->AddPage(p, getPageTitle(*p), true);
	}
}

void PropertyDlg::OnNodeTooltip(wxTreeEvent &event)
{
}

void PropertyDlg::OnSearch(wxCommandEvent &event)
{
}

void PropertyDlg::OnPageChanged(wxBookCtrlEvent &event)
{
}

void PropertyDlg::OnPageChanging(wxBookCtrlEvent &event)
{
}

void PropertyDlg::handleNotification(PropertyPagePanel &page)
{
	if (!page.isActive())
		return;

	m_propertyPages->SetPageText(0, getPageTitle(page));
}
