#pragma once

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>

wxDECLARE_EVENT(EVT_EDIT_TEXT, wxCommandEvent);
wxDECLARE_EVENT(EVT_EDIT_ENTER, wxCommandEvent);
wxDECLARE_EVENT(EVT_EDIT_MAXLEN, wxCommandEvent);
wxDECLARE_EVENT(EVT_EDIT_BTN, wxCommandEvent);

#define IDC_EDIT_TXT 1000
#define IDC_EDIT_BTN 1001

///////////////////////////////////////////////////////////////////////////////
/// Class EditBtn
///////////////////////////////////////////////////////////////////////////////
class EditBtn
: public wxPanel
{
public:
	EditBtn(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &label = wxEmptyString, const wxString &buttonLabel = wxEmptyString, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(478, 51), long style = wxTAB_TRAVERSAL, const wxString &name = wxEmptyString);
	~EditBtn();

	void setLabel(const wxString &text);
	wxString getLabel(void) const;

	void setButtonLabel(const wxString &text);
	wxString getButtonLabel(void) const;

	void setValue(const wxString &text);
	wxString getValue(void) const;
	wxTextCtrl *getEdit(void);

protected:
	void OnText(wxCommandEvent &event);
	void OnEnter(wxCommandEvent &event);
	void OnMaxLen(wxCommandEvent &event);
	void OnButton(wxCommandEvent &event);

private:
	void Init(void);

private:
	wxStaticText *m_editLbl;
	wxTextCtrl *m_editTxt;
	wxButton *m_editBtn;

	wxDECLARE_EVENT_TABLE();
};
