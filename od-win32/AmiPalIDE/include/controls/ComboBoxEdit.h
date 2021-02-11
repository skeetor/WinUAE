#pragma once

#include <wx/combobox.h>

wxDECLARE_EVENT(EVT_COMBO_CHOICE, wxCommandEvent);

class ComboBoxEdit
: public wxComboBox
{
public:
	ComboBoxEdit(wxWindow *parent, wxWindowID id,
		bool addText,		// If true, then a text which has been entered is added to the listbox
		const wxString &value = wxEmptyString,
		const wxSize& size = wxDefaultSize,
		const wxValidator &validator = wxDefaultValidator
	);
	~ComboBoxEdit() override;

public:
	void OnSelection(wxCommandEvent &event);
	void OnEnter(wxCommandEvent &event);

private:
	bool m_addText:1;

	wxDECLARE_EVENT_TABLE();
};
