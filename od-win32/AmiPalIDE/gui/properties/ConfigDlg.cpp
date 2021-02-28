#include "gui/properties/ConfigDlg.h"

#include "gui/properties/pages/GlobalSettingsPage.h"
#include "gui/properties/pages/DebuggerViewPage.h"

enum
{
	IDD_UNUSED = wxID_HIGHEST

	,IDD_GLOBAL_SETTINGS
};

ConfigDlg::ConfigDlg(wxWindow *parent)
: PropertyDlg(parent, wxT("Configuration"), wxT("AmiPal IDE Settings"), wxID_ANY, wxDefaultPosition, wxSize(830, 468))
{
	initSections();
}

ConfigDlg::~ConfigDlg(void)
{
}

void ConfigDlg::initSections(void)
{
	wxTreeItemId root = getRootId();

	createApplicationSection(root);
	createDebuggerSection(root);
	createKeyboardSection(root);
}

void ConfigDlg::createApplicationSection(wxTreeItemId &root)
{
	PropertyPages *pages = getPropertyPages();

	wxTreeItemId item = registerPropertyPage(root, wxT("Application"), new GlobalSettingsPage(pages));
	getPropertyTree()->SelectItem(item);
}

void ConfigDlg::createDebuggerSection(wxTreeItemId &root)
{
	PropertyPages *pages = getPropertyPages();

	registerPropertyPage(root, wxT("Debugger"), new DebuggerViewPage(pages));
}

void ConfigDlg::createKeyboardSection(wxTreeItemId &root)
{
/*	PropertyPages *pages = getPropertyPages();

	registerPropertyPage(root, wxT("Keyboard shortcuts"), new PropertyPagePanel(pages, wxT("Assignments")));*/
}
