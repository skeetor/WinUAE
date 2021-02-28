#include "gui/properties/pages/DebuggerViewPage.h"

#include <wx/gbsizer.h>
#include <wx/stattext.h>

wxBEGIN_EVENT_TABLE(DebuggerViewPage, wxPanel)

	EVT_FONTPICKER_CHANGED(IDC_MEMORY_VIEW_FONT, DebuggerViewPage::OnMemoryViewFont)
	EVT_CHECKBOX(IDC_CONFIRM_DELETE_BREAKPOINTS, DebuggerViewPage::OnConfirmBreakpoints)

wxEND_EVENT_TABLE()

DebuggerViewPage::DebuggerViewPage(wxWindow *parent, wxWindowID id)
: PropertyPagePanel(parent, wxT("Debugger"), id)
{
	init();
	reset();
}

DebuggerViewPage::~DebuggerViewPage()
{
}

void DebuggerViewPage::init()
{
	wxGridBagSizer *sizer;
	sizer = new wxGridBagSizer(0, 0);
	sizer->SetFlexibleDirection(wxBOTH);
	sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_askBreakpointsChk = new wxCheckBox(this, IDC_CONFIRM_DELETE_BREAKPOINTS, wxT("Confirm deleting all breakpoints"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(m_askBreakpointsChk, wxGBPosition(0, 0), wxGBSpan(1, 2), wxALL, 5);

	wxStaticText *st;
	st = new wxStaticText(this, wxID_ANY, wxT("Column Font"), wxDefaultPosition, wxDefaultSize, 0);
	st->Wrap(-1);
	sizer->Add(st, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_fontPicker = new wxFontPickerCtrl(this, IDC_MEMORY_VIEW_FONT, DebuggerConfig::getInstance().memoryViewFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE);
	m_fontPicker->SetMaxPointSize(100);
	sizer->Add(m_fontPicker, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5);

	sizer->AddGrowableCol(1);

	SetSizer(sizer);
	Layout();
}

void DebuggerViewPage::updateSettings()
{
	m_askBreakpointsChk->SetValue(m_config.askOnDeleteAllBreakpoints);
	m_fontPicker->SetFont(m_config.memoryViewFont);
}

void DebuggerViewPage::apply(void)
{
	DebuggerConfig::getInstance().update(m_config);
}

void DebuggerViewPage::reset(void)
{
	m_config.update(DebuggerConfig::getInstance());
	updateSettings();
}

void DebuggerViewPage::OnMemoryViewFont(wxFontPickerEvent &event)
{
	m_config.memoryViewFont = m_fontPicker->GetSelectedFont();
	setDirty(true);
}

void DebuggerViewPage::OnConfirmBreakpoints(wxCommandEvent &event)
{
	m_config.askOnDeleteAllBreakpoints = m_askBreakpointsChk->GetValue();
	setDirty(true);
}
