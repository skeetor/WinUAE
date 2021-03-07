#pragma once

#include "config/ApplicationConfig.h"
#include "gui/properties/PropertyPagePanel.h"

#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>

#define IDC_CLOSEBUTTON_ACTION 1000
#define IDC_CLOSE_BUTTON_ACTION 1001
#define IDC_SAVE_LAYOUT_TXT 1002
#define IDC_SAVE_LAYOUT_CHK 1003
#define IDC_SAVE_POSITION 1004

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
	void OnLayoutName(wxCommandEvent &event);
	void OnSaveLayout(wxCommandEvent &event);
	void OnSavePosition(wxCommandEvent &event);

	void updateSettings(void);

private:
	void init(void);

private:
	wxChoice *m_closeButtonList;
	wxTextCtrl *m_saveLayoutTxt;
	wxCheckBox *m_saveLayoutChk;
	wxCheckBox *m_savePositionChk;

	ApplicationConfig m_config;

	wxDECLARE_EVENT_TABLE();
};
