#include "controls/EditBtn.h"

wxBEGIN_EVENT_TABLE(EditBtn, wxPanel)

	EVT_TEXT(IDC_EDIT_TXT, EditBtn::OnText)
	EVT_TEXT_ENTER(IDC_EDIT_TXT, EditBtn::OnEnter)
	EVT_TEXT_MAXLEN(IDC_EDIT_TXT, EditBtn::OnMaxLen)
	EVT_BUTTON(IDC_EDIT_BTN, EditBtn::OnButton)

wxEND_EVENT_TABLE()

wxDEFINE_EVENT(EVT_EDIT_TEXT, wxCommandEvent);
wxDEFINE_EVENT(EVT_EDIT_ENTER, wxCommandEvent);
wxDEFINE_EVENT(EVT_EDIT_MAXLEN, wxCommandEvent);
wxDEFINE_EVENT(EVT_EDIT_BTN, wxCommandEvent);

EditBtn::EditBtn(wxWindow *parent, wxWindowID id, const wxString &label, const wxString &buttonLabel, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
: wxPanel(parent, id, pos, size, style, name)
{
	Init();

	m_editLbl->SetLabel(label);
	m_editBtn->SetLabel(buttonLabel);
}

EditBtn::~EditBtn()
{
}

void EditBtn::Init()
{
	wxFlexGridSizer *sizer;
	sizer = new wxFlexGridSizer(1, 3, 0, 0);
	sizer->AddGrowableCol(1);
	sizer->SetFlexibleDirection(wxHORIZONTAL);
	sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_editLbl = new wxStaticText(this, wxID_ANY, wxT("Input"), wxDefaultPosition, wxDefaultSize, 0);
	m_editLbl->Wrap(-1);
	sizer->Add(m_editLbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_editTxt = new wxTextCtrl(this, IDC_EDIT_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sizer->Add(m_editTxt, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_TOP | wxALL | wxEXPAND, 5);

	m_editBtn = new wxButton(this, IDC_EDIT_BTN, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(m_editBtn, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	SetSizer(sizer);
	Layout();
}

void EditBtn::OnText(wxCommandEvent &event)
{
	wxCommandEvent evt(EVT_EDIT_TEXT, GetId());

	evt.SetEventObject(this);
	evt.SetString(m_editTxt->GetValue());

	ProcessWindowEvent(evt);
}

void EditBtn::OnEnter(wxCommandEvent &event)
{
	wxCommandEvent evt(EVT_EDIT_ENTER, GetId());

	evt.SetEventObject(this);
	evt.SetString(m_editTxt->GetValue());

	ProcessWindowEvent(evt);
}

void EditBtn::OnMaxLen(wxCommandEvent &event)
{
	wxCommandEvent evt(EVT_EDIT_MAXLEN, GetId());

	evt.SetEventObject(this);
	evt.SetString(m_editTxt->GetValue());

	ProcessWindowEvent(evt);
}

void EditBtn::OnButton(wxCommandEvent &event)
{
	wxCommandEvent evt(EVT_EDIT_BTN, GetId());

	evt.SetEventObject(this);
	evt.SetString(m_editTxt->GetValue());

	ProcessWindowEvent(evt);
}

void EditBtn::setLabel(const wxString &text)
{
	m_editLbl->SetLabel(text);
}

wxString EditBtn::getLabel(void) const
{
	return m_editLbl->GetLabel();
}

void EditBtn::setButtonLabel(const wxString &text)
{
	m_editBtn->SetLabel(text);
}

wxString EditBtn::getButtonLabel(void) const
{
	return m_editBtn->GetLabel();
}

void EditBtn::setValue(const wxString &text)
{
	m_editTxt->SetLabel(text);
}

wxString EditBtn::getValue(void) const
{
	return m_editTxt->GetLabel();
}

wxTextCtrl *EditBtn::getEdit(void)
{
	return m_editTxt;
}
