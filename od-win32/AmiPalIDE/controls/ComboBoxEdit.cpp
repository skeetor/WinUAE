#include "controls/ComboBoxEdit.h"

wxBEGIN_EVENT_TABLE(ComboBoxEdit, wxComboBox)

	EVT_COMBOBOX(wxID_ANY, OnSelection)
	EVT_TEXT_ENTER(wxID_ANY, OnEnter)

wxEND_EVENT_TABLE()

wxDEFINE_EVENT(EVT_COMBO_CHOICE, wxCommandEvent);

ComboBoxEdit::ComboBoxEdit(wxWindow *parent, wxWindowID id,
	bool addText,
	const wxString &value,
	const wxSize& size,
	const wxValidator &validator)
: wxComboBox(parent, id, value, wxDefaultPosition, size, 0, nullptr, wxTE_PROCESS_ENTER|wxBORDER_RAISED, validator, wxASCII_STR(wxComboBoxNameStr))
, m_addText(addText)
{
}

ComboBoxEdit::~ComboBoxEdit()
{
}

void ComboBoxEdit::OnSelection(wxCommandEvent &event)
{
	wxCommandEvent evt(EVT_COMBO_CHOICE, GetId());

	evt.SetEventObject(this);
	evt.SetInt(event.GetSelection());
	evt.SetString(GetValue());

	ProcessWindowEvent(evt);
}

void ComboBoxEdit::OnEnter(wxCommandEvent &event)
{
	wxCommandEvent evt(EVT_COMBO_CHOICE, GetId());

	int sel = event.GetSelection();
	if (sel == -1 && m_addText)
		sel = Append(GetValue());

	evt.SetEventObject(this);
	evt.SetInt(sel);
	evt.SetString(GetValue());

	ProcessWindowEvent(evt);
}
