#include "gui/MemoryPanel.h"
#include "gui/MemoryToolBar.h"

#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>

#include <string.h>

#include "controls/EditBtn.h"

wxBEGIN_EVENT_TABLE(MemoryPanel, wxPanel)

	EVT_SIZE(MemoryPanel::OnSize)

	// Custom events
	EVT_COMMAND(IDC_ADDRESS_TXT, EVT_EDIT_ENTER, MemoryPanel::OnAddressChanged)
	EVT_COMMAND(IDC_ADDRESS_TXT, EVT_EDIT_BTN, MemoryPanel::OnLock)

wxEND_EVENT_TABLE()

MemoryPanel::MemoryPanel(MemoryToolBar *toolBar, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
: wxPanel(parent, id, pos, size, style, name)
, m_toolBar(toolBar)
, m_horizSB(wxSystemMetric::wxSYS_HSCROLL_Y)
, m_vertSB(wxSystemMetric::wxSYS_VSCROLL_X)
, m_address(0)
, m_curAddress(0)
, m_addressRep(wxT("$00000000"))
, m_memorySize(0)
, m_displayColumns(0)
, m_columns(0)
, m_curLines(0)
, m_bytes(2)
, m_type(DisplayType::DST_HEX)
, m_addressLimit(0xffffffff)
, m_spaces(true)
{
	Init();

	DebuggerConfig::getInstance().addListener(this);
	m_addressTxt->setValue(wxT("$00000000"));
}

MemoryPanel::~MemoryPanel()
{
	DebuggerConfig::getInstance().removeListener(this);
}

void MemoryPanel::Init(void)
{
	wxGridBagSizer *sizer;
	sizer = new wxGridBagSizer(0, 0);
	sizer->SetFlexibleDirection(wxBOTH);
	sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_addressTxt = new EditBtn(this, IDC_ADDRESS_TXT, wxT("Address"), wxT("Lock"), wxDefaultPosition, wxDefaultSize, 0);
	sizer->Add(m_addressTxt, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5);

	m_memoryTxt = new wxTextCtrl(this, IDC_MEMORY_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE | wxTE_NOHIDESEL | wxTE_PROCESS_ENTER | wxTE_READONLY | wxBORDER_STATIC);
	m_memoryTxt->SetFont(DebuggerConfig::getInstance().memoryViewFont);
	m_memoryTxt->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	m_memoryTxt->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	sizer->Add(m_memoryTxt, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 5);

	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(1);

	SetSizer(sizer);
	Layout();
	lock(isLocked());
}

bool MemoryPanel::serialize(wxString const &groupId, wxConfigBase *config)
{
	return true;
}

bool MemoryPanel::deserialize(wxString const &groupId, wxConfigBase *config)
{
	return true;
}

void MemoryPanel::OnAddressChanged(wxCommandEvent &event)
{
	wxString addr = event.GetString();

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

	setAddressRep(addr);
	setAddress(val);
}

void MemoryPanel::OnLock(wxCommandEvent &event)
{
	lock(!isLocked());
}

void MemoryPanel::setAddress(size_t address)
{
	if (m_addressLimit)
	{
		if (address > m_addressLimit)
			address %= (m_addressLimit+1);
	}

	m_address = address;
	m_curAddress = address;
	printDump(m_curAddress);
}

void MemoryPanel::setAddressRep(const wxString &address)
{
	m_addressRep = address;
}

void MemoryPanel::setMemorySize(uint32_t size)
{
	m_memorySize = size;
	printDump(m_curAddress);
}

void MemoryPanel::setColumns(uint32_t columns)
{
	m_displayColumns = columns;
	m_columns = columns;

	printDump(m_curAddress);
}

void MemoryPanel::setBytes(uint32_t bytes)
{
	m_bytes = bytes;
	printDump(m_curAddress);
}

void MemoryPanel::setType(DisplayType type)
{
	m_type = type;
	printDump(m_curAddress);
}

void MemoryPanel::setSpaces(bool spaces)
{
	m_spaces = spaces;
	printDump(m_curAddress);
}

void MemoryPanel::lock(bool locked)
{
	Document::lock(locked);
	m_toolBar->lock(!locked);
	m_addressTxt->Enable(!locked, true);

	// TODO: Update Lockbutton bitmap
	if (locked)
		m_addressTxt->setButtonLabel(wxT("Unlock"));
	else
		m_addressTxt->setButtonLabel(wxT("Lock"));
}

void MemoryPanel::activate(void)
{
	m_toolBar->setMemoryPanel(this);
	m_toolBar->Enable();
	m_toolBar->lock(isLocked());
	m_toolBar->updateControls();
}

void MemoryPanel::deactivate(void)
{
	m_toolBar->setMemoryPanel(nullptr);
	m_toolBar->Disable();
}

bool MemoryPanel::closing(bool forced, bool active)
{
	if (active)
		deactivate();

	// No objections to closing.
	return true;
}

void MemoryPanel::adjustDimensions(void)
{
	int aw;
	int ah;
	m_addressTxt->GetSize(&aw, &ah);

	wxSize sz = GetClientSize();
	int cw;
	int ch;

	m_memoryTxt->GetTextExtent("0", &cw, &ch);
	int chars = ((sz.GetWidth() - m_vertSB) / cw);
	int lines = ((sz.GetHeight() - ah - m_horizSB) / ch) - 1;
	if (lines <= 0)
		lines = 1;
	m_curLines = lines;

	if (m_displayColumns)
	{
		m_columns = m_displayColumns;
		return;
	}

	// Address(8) + ': '(2) + ....' 'ABCD(1)+ Scrollbar(3)
	static const int fixedChars = 14;
	if (chars <= fixedChars)
	{
		m_columns = 1;
		return;
	}

	chars -= fixedChars;

	int columnWidth;
	if (m_type == DisplayType::DST_BINARY)
		columnWidth = 8;
	else
		columnWidth = 2;

	columnWidth *= m_bytes;

	// Account for ascii char(+1)
	columnWidth += m_bytes;
	if (m_spaces)
		columnWidth++;

	int columns = chars / columnWidth;
	if (columns <= 0)
		columns = 1;

	m_columns = columns;
}

void MemoryPanel::OnSize(wxSizeEvent &event)
{
	printDump(m_curAddress);

	event.Skip();
}

size_t MemoryPanel::printDump(size_t address)
{
	adjustDimensions();

	DbgByte *memPtr;
	size_t size;

	// If the panel is locked, then we use the memorydump for the cache.
	// In that case we still update the view, as some
	// display options might have been changed.
	if (!isLocked())
	{
		size = m_curLines * m_columns * m_bytes;

		if (m_memorySize != 0 && size > m_memorySize)
			size = m_memorySize;

		m_lockCache.resize(size);
		memPtr = &m_lockCache[0];
		size = Debugger->MemoryRead(address, memPtr, size);
		m_lockCache.resize(size);
	}
	else
		size = m_lockCache.size();

	if (!size)
		return 0;

	memPtr = &m_lockCache[0];

	if (m_lockCache.empty())
		return 0;

	wxString lines;

	for (uint32_t l = 0; l < m_curLines; l++)
	{
		wxString line;
		wxString ascii;

		line.Printf(wxT("%08llx: "), address);

		for (uint32_t col = 0; col < m_columns; col++)
		{
			for (uint32_t b = 0; b < m_bytes; b++)
			{
				wxString s;

				uint8_t c = memPtr->value;
				if (m_type == DisplayType::DST_BINARY)
				{
					if (memPtr->safe)
					{
						for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
						{
							if (c & mask)
								s += '1';
							else
								s += '0';
						}
					}
					else
						s += wxT("????????");
				}
				else
				{
					if (memPtr->safe)
						s.Printf(wxT("%02x"), c);
					else
						s += wxT("??");
				}

				if (memPtr->safe)
				{
					if (c < 32)
						ascii += '.';
					else
						ascii += c;
				}
				else
					ascii += wxT("?");

				memPtr++;
				line += s;
				address++;

				if (m_addressLimit)
					address %= (m_addressLimit+1);
			}

			if (m_spaces)
				line += ' ';
		}

		lines += line;
		lines += ' ';
		lines += ascii;
		lines += '\n';
	}

	// Remove the last newline, so we end cleanly without an empty line at the bottom.
	lines.RemoveLast();

	m_memoryTxt->SetValue(lines);

	return address;
}

void MemoryPanel::handleNotification(DebuggerConfig &config)
{
	m_memoryTxt->SetFont(DebuggerConfig::getInstance().memoryViewFont);
	printDump(m_curAddress);
	m_memoryTxt->Refresh();
}
