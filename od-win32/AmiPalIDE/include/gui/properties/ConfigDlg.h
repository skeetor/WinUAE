#pragma once

#include "gui/properties/PropertyDlg.h"

class GlobalSettingsPage;

class ConfigDlg
: public PropertyDlg
{
public:
	ConfigDlg(wxWindow *parent);
	~ConfigDlg(void) override;

private:
	void initSections(void);

	void createApplicationSection(wxTreeItemId &root);
	void createDebuggerSection(wxTreeItemId &root);
	void createKeyboardSection(wxTreeItemId &root);
};
