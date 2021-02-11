#pragma once

#include "controls/ComboBoxEdit.h"

#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/aui/aui.h>

///////////////////////////////////////////////////////////////////////////

#define IDC_COLUMNS_BOX 1008
#define IDC_BYTES_BOX 1009
#define IDC_TYPE_BOX 1010
#define IDC_SIZE_TXT 1011
#define IDC_SPACES_CHK 1012
#define IDC_MEMORY_NEW 1013

class MainFrame;
class MemoryPanel;

class MemoryToolBar
: public wxAuiToolBar
{
public:
	MemoryToolBar(MainFrame*frame, wxWindow *parent,
		wxWindowID id = wxID_ANY,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = wxAUI_TB_DEFAULT_STYLE);
	~MemoryToolBar() override;

public: // overrides
	bool Enable(bool enable = true) override;

public:
	void setMemoryPanel(MemoryPanel *panel) { m_memory = panel; }

	void reset(void);

	// Set the controls to the values from the current MemoryPanel
	// If MemoryPanel is not set, it will reset to defaults.
	void updateControls(void);

protected:
	void OnNewMemory(wxCommandEvent &event);

	void OnColumnsChanged(wxCommandEvent &event);
	void OnBytesChanged(wxCommandEvent &event);
	void OnType(wxCommandEvent &event);
	void OnSpaces(wxCommandEvent &event);
	void OnAddress(wxCommandEvent &event);
	void OnAddressUpdate(wxCommandEvent &event);
	void OnMemorySize(wxCommandEvent &event);
	void OnMemorySizeUpdate(wxCommandEvent &event);

private:
	void updateColumnsBox();
	void updateBytesBox();

private:
	MainFrame*m_frame;
	MemoryPanel *m_memory;

	wxComboBox *m_columnsBox;
	wxComboBox *m_bytesBox;
	wxComboBox *m_typeBox;
	wxTextCtrl *m_sizeTxt;
	wxCheckBox *m_spacesChk;

	wxDECLARE_EVENT_TABLE();
};
