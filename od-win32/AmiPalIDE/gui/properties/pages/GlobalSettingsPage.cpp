#include "gui/properties/pages/GlobalSettingsPage.h"
#include "config/ApplicationConfig.h"

#include <wx/gbsizer.h>
#include <wx/stattext.h>

wxBEGIN_EVENT_TABLE(GlobalSettingsPage, wxPanel)

	EVT_CHOICE(IDC_CLOSE_BUTTON_ACTION, GlobalSettingsPage::OnCloseAction)
	EVT_TEXT_ENTER(IDC_SAVE_LAYOUT_TXT, GlobalSettingsPage::OnSaveLayoutTxt)
	EVT_CHECKBOX(IDC_SAVE_LAYOUT_CHK, GlobalSettingsPage::OnSaveLayoutChk)

wxEND_EVENT_TABLE()

GlobalSettingsPage::GlobalSettingsPage(wxWindow *parent, wxWindowID id)
: PropertyPagePanel(parent, wxT("Global Settings"), id)
{
	init();
	reset();
}

GlobalSettingsPage::~GlobalSettingsPage(void)
{
}

void GlobalSettingsPage::init(void)
{
	wxGridBagSizer *sizer;
	sizer = new wxGridBagSizer(0, 0);
	sizer->SetFlexibleDirection(wxBOTH);
	sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText *st;
	st = new wxStaticText(this, IDC_CLOSEBUTTON_ACTION, wxT("Frame Close Button"), wxDefaultPosition, wxDefaultSize, 0);
	st->Wrap(-1);
	sizer->Add(st, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_saveLayoutTxt = new wxTextCtrl(this, IDC_SAVE_LAYOUT_TXT, wxT("default"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sizer->Add(m_saveLayoutTxt, wxGBPosition(2, 1), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5);

	m_saveLayoutChk = new wxCheckBox(this, IDC_SAVE_LAYOUT_CHK, wxT("Save layout on exit"), wxDefaultPosition, wxDefaultSize, 0);
	m_saveLayoutChk->SetValue(true);
	sizer->Add(m_saveLayoutChk, wxGBPosition(2, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString m_closeButtonListChoices[] = { wxT("Close Window"), wxT("Minimize Window"), wxT("Ignore") };
	int m_closeButtonListNChoices = sizeof(m_closeButtonListChoices) / sizeof(wxString);
	m_closeButtonList = new wxChoice(this, IDC_CLOSE_BUTTON_ACTION, wxDefaultPosition, wxDefaultSize, m_closeButtonListNChoices, m_closeButtonListChoices, 0);
	m_closeButtonList->SetSelection(0);
	sizer->Add(m_closeButtonList, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);

	sizer->AddGrowableCol(1);

	SetSizer(sizer);
	Layout();
}

void GlobalSettingsPage::reset(void)
{
	m_config.update(ApplicationConfig::getInstance());
	updateSettings();
}

void GlobalSettingsPage::updateSettings()
{
	switch (m_config.closeAction)
	{
		case CloseButtonAction::CLOSE_WINDOW:
			m_closeButtonList->SetSelection(0);
		break;

		case CloseButtonAction::CLOSE_MINIMIZE:
			m_closeButtonList->SetSelection(1);
		break;

		case CloseButtonAction::CLOSE_IGNORE:
			m_closeButtonList->SetSelection(2);
		break;
	}

	m_saveLayoutTxt->SetValue(m_config.layout);
	bool enable = m_config.saveLayout;

	m_saveLayoutChk->SetValue(enable);
	m_saveLayoutTxt->Enable(enable);
}

void GlobalSettingsPage::OnSaveLayoutTxt(wxCommandEvent &event)
{
	wxString s = m_saveLayoutTxt->GetValue();
	if (s == m_config.layout)
		return;

	m_config.layout = s;
	setDirty(true);
}

void GlobalSettingsPage::OnSaveLayoutChk(wxCommandEvent &event)
{
	m_config.saveLayout = m_saveLayoutChk->GetValue();
	m_saveLayoutTxt->Enable(m_config.saveLayout);
	setDirty(true);
}

void GlobalSettingsPage::OnCloseAction(wxCommandEvent &event)
{
	switch (m_closeButtonList->GetSelection())
	{
		case 0:
			m_config.closeAction = CloseButtonAction::CLOSE_WINDOW;
		break;

		case 1:
			m_config.closeAction = CloseButtonAction::CLOSE_MINIMIZE;
		break;

		case 2:
			m_config.closeAction = CloseButtonAction::CLOSE_IGNORE;
		break;
	}

	setDirty(true);
}

void GlobalSettingsPage::apply(void)
{
	ApplicationConfig::getInstance().update(m_config);
}
