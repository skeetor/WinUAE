#pragma once

#include "config/ApplicationConfig.h"
#include "gui/properties/PropertyPagePanel.h"

#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>

#define IDC_CLOSEBUTTON_ACTION 1000
#define IDC_SAVE_LAYOUT_TXT 1001
#define IDC_SAVE_LAYOUT_CHK 1002
#define IDC_CLOSE_BUTTON_ACTION 1003

class GlobalSettingsPage
: public PropertyPagePanel
{
public:
	GlobalSettingsPage(wxWindow *parent, wxWindowID id = wxID_ANY);
	~GlobalSettingsPage(void);

public: // PropertyPage
	void apply(void) override;
	void reset(void) override;

protected:
	void OnCloseAction(wxCommandEvent &event);
	void OnSaveLayoutTxt(wxCommandEvent &event);
	void OnSaveLayoutChk(wxCommandEvent &event);

	void updateSettings(void);

private:
	void init(void);

private:
	wxTextCtrl *m_saveLayoutTxt;
	wxCheckBox *m_saveLayoutChk;
	wxChoice *m_closeButtonList;

	ApplicationConfig m_config;

	wxDECLARE_EVENT_TABLE();
};
