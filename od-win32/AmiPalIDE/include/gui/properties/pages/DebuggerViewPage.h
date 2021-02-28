#pragma once

#include "gui/properties/PropertyPagePanel.h"
#include "config/DebuggerConfig.h"

#include <wx/fontpicker.h>
#include <wx/checkbox.h>

#define IDC_CONFIRM_DELETE_BREAKPOINTS 1000
#define IDC_MEMORY_VIEW_FONT 1001

class DebuggerViewPage
: public PropertyPagePanel
{
public:
	DebuggerViewPage(wxWindow *parent, wxWindowID id = wxID_ANY);
	~DebuggerViewPage();

public: // PropertyPage
	void apply(void) override;
	void reset(void) override;

protected:
	void OnMemoryViewFont(wxFontPickerEvent &event);
	void OnConfirmBreakpoints(wxCommandEvent &event);

	void updateSettings(void);

private:
	void init();

private:
	wxCheckBox *m_askBreakpointsChk;
	wxFontPickerCtrl *m_fontPicker;

	DebuggerConfig m_config;

	wxDECLARE_EVENT_TABLE();
};
