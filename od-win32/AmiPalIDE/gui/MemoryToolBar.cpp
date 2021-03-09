
#include "gui/MemoryToolBar.h"
#include "gui/MainFrame.h"
#include "gui/panels/MemoryPanel.h"

#include <wx/stattext.h>

wxBEGIN_EVENT_TABLE(MemoryToolBar, wxAuiToolBar)

	EVT_BUTTON(IDC_MEMORY_NEW, MemoryToolBar::OnNewMemory)

	EVT_COMBOBOX(IDC_TYPE_BOX, MemoryToolBar::OnType)
	EVT_CHECKBOX(IDC_SPACES_CHK, MemoryToolBar::OnSpaces)

	EVT_TEXT_ENTER(IDC_ADDRESS_TXT, MemoryToolBar::OnAddress)
	EVT_TEXT(IDC_ADDRESS_TXT, MemoryToolBar::OnAddressUpdate)

	EVT_TEXT_ENTER(IDC_SIZE_TXT, MemoryToolBar::OnMemorySize)
	EVT_TEXT(IDC_SIZE_TXT, MemoryToolBar::OnMemorySizeUpdate)

	// Custom events
	EVT_COMMAND(IDC_COLUMNS_BOX, EVT_COMBO_CHOICE, MemoryToolBar::OnColumnsChanged)
	EVT_COMMAND(IDC_BYTES_BOX, EVT_COMBO_CHOICE, MemoryToolBar::OnBytesChanged)

wxEND_EVENT_TABLE()

MemoryToolBar::MemoryToolBar(MainFrame *frame, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
: wxAuiToolBar(parent, id, pos, size, style)
, DocumentWindow("MemoryToolBar", this)
, m_frame(frame)
, m_memory(nullptr)
{
	wxStaticText *st;

	st = new wxStaticText(this, wxID_ANY, wxT("Columns:"), wxDefaultPosition, wxDefaultSize, 0);
	st->Wrap(-1);
	AddControl(st);
	m_columnsBox = new ComboBoxEdit(this, IDC_COLUMNS_BOX, true, wxT("Auto"));
	m_columnsBox->Append(wxT("Auto"));
	m_columnsBox->Append(wxT("8"));
	m_columnsBox->Append(wxT("16"));
	m_columnsBox->SetSelection(0);
	AddControl(m_columnsBox);

	st = new wxStaticText(this, wxID_ANY, wxT("Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	st->Wrap(-1);
	AddControl(st);
	m_bytesBox = new ComboBoxEdit(this, IDC_BYTES_BOX, true, wxT("3"), wxSize(50, -1));
	m_bytesBox->Append(wxT("1"));
	m_bytesBox->Append(wxT("2"));
	m_bytesBox->Append(wxT("4"));
	m_bytesBox->SetSelection(1);
	AddControl(m_bytesBox);

	st = new wxStaticText(this, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0);
	st->Wrap(-1);
	AddControl(st);
	m_typeBox = new wxComboBox(this, IDC_TYPE_BOX, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_typeBox->Append(wxT("Hex"));
	m_typeBox->Append(wxT("Binary"));
	m_typeBox->SetSelection(0);
	AddControl(m_typeBox);

	st = new wxStaticText(this, wxID_ANY, wxT("Size"), wxDefaultPosition, wxDefaultSize, 0);
	st->Wrap(-1);
	AddControl(st);
	m_sizeTxt = new wxTextCtrl(this, IDC_SIZE_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_STATIC);
	AddControl(m_sizeTxt);

	m_spacesChk = new wxCheckBox(this, IDC_SPACES_CHK, wxT("Spaces"), wxDefaultPosition, wxDefaultSize, 0);
	m_spacesChk->SetValue(true);
	AddControl(m_spacesChk);
	wxButton *btn;
	btn = new wxButton(this, IDC_MEMORY_NEW, wxT("New"), wxDefaultPosition, wxDefaultSize, 0);
	AddControl(btn);

	Realize();
}

MemoryToolBar::~MemoryToolBar()
{
}

bool MemoryToolBar::serialize(wxString const &groupId, wxConfigBase *config)
{
	return true;
}

bool MemoryToolBar::deserialize(wxString const &groupId, wxConfigBase *config)
{
	return true;
}

void MemoryToolBar::reset(void)
{
	m_sizeTxt->SetValue("");
	bool enable = true;
	if (m_memory)
		enable = m_memory->isLocked();
	m_sizeTxt->Enable(enable);

	m_columnsBox->SetSelection(0);
	m_bytesBox->SetSelection(1);
	m_typeBox->SetSelection(0);
	m_spacesChk->SetValue(true);
}

void MemoryToolBar::updateColumnsBox()
{
	wxString s;

	uint32_t val = m_memory->getColumns();
	if (val)
	{
		s = "";
		s << val;
	}
	else
		s = "Auto";

	int pos = m_columnsBox->FindString(s);
	if (pos == wxNOT_FOUND)
		pos = m_columnsBox->Append(s);

	m_columnsBox->SetSelection(pos);
}

void MemoryToolBar::updateBytesBox()
{
	wxString s;

	uint32_t val = m_memory->getColumnBytes();
	if (!val)
		val = 1;

	s << val;

	int pos = m_bytesBox->FindString(s);
	if (pos == wxNOT_FOUND)
		pos = m_bytesBox->Append(s);

	m_bytesBox->SetSelection(pos);
}

void MemoryToolBar::lock(bool locked)
{
	m_sizeTxt->Enable(locked);
}

void MemoryToolBar::updateControls(void)
{
	if (m_memory)
	{
		wxString s;

		uint32_t val = m_memory->getMemorySize();
		if (val)
			s << m_memory->getMemorySize();
		m_sizeTxt->SetValue(s);
		m_sizeTxt->Enable(!m_memory->isLocked());

		updateColumnsBox();
		updateBytesBox();

		m_typeBox->SetSelection(m_memory->getType());
		m_spacesChk->SetValue(m_memory->hasSpaces() != false);
	}
	else
		reset();
}
 
bool MemoryToolBar::Enable(bool enable)
{
	m_columnsBox->Enable(enable);
	m_spacesChk->Enable(enable);
	m_bytesBox->Enable(enable);
	m_typeBox->Enable(enable);

	if (m_memory)
		enable = !m_memory->isLocked();
	m_sizeTxt->Enable(enable);

	return true;
}

void MemoryToolBar::OnNewMemory(wxCommandEvent &event)
{
	wxCommandEvent btn(IDM_DEBUG_MEMORY);

	m_frame->OnMemory(btn);
}

void MemoryToolBar::OnColumnsChanged(wxCommandEvent &event)
{
	wxString s = event.GetString();

	uint32_t val = 0;
	if (s != wxT("Auto"))
		val = strtod(s, nullptr);

	m_memory->setColumns(val);
}

void MemoryToolBar::OnBytesChanged(wxCommandEvent &event)
{
	wxString s = event.GetString();
	uint32_t val = 0;
	val = strtod(s, nullptr);

	m_memory->setColumnBytes(val);
}

void MemoryToolBar::OnType(wxCommandEvent &event)
{
	MemoryPanel::DisplayType t;

	if (m_typeBox->GetSelection() != 1)
		t = MemoryPanel::DisplayType::DST_HEX;
	else
		t = MemoryPanel::DisplayType::DST_BINARY;

	m_memory->setType(t);
}

void MemoryToolBar::OnSpaces(wxCommandEvent &event)
{
	m_memory->setSpaces(m_spacesChk->IsChecked());
}

void MemoryToolBar::OnAddress(wxCommandEvent &event)
{
	// TODO: The address can be determined by using symbols or references
	wxString s = event.GetString();
	size_t val = 0;

	const char *p = s;
	size_t len = s.length();

	if (len > 1)
	{
		if (*p == '$')
		{
			p++;
			len--;
		}
		else if (*p == '0' && p[1] == 'x')
		{
			p += 2;
			len -= 2;
		}
	}

	val = strtoull(p, nullptr, 16);

	m_memory->setAddress(val);
	m_memory->setAddressRep(s);
}

void MemoryToolBar::OnAddressUpdate(wxCommandEvent &event)
{
	if (!m_memory)
		return;

	wxString s = event.GetString();
	m_memory->setAddressRep(s);
}

void MemoryToolBar::OnMemorySize(wxCommandEvent &event)
{
	wxString s = event.GetString();
	uint32_t val = 0;
	val = strtod(s, nullptr);

	m_memory->setMemorySize(val);
}

void MemoryToolBar::OnMemorySizeUpdate(wxCommandEvent &event)
{
	if (!m_memory)
		return;

	wxString s = event.GetString();
	uint32_t val = 0;
	val = strtod(s, nullptr);
	m_memory->setMemorySize(val);
}
