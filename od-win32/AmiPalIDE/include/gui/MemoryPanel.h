#pragma once

#include <wx/control.h>
#include <wx/panel.h>

#include "gui/MemoryToolBar.h"
#include "gui/DocumentView.h"
#include "debugger/DebuggerAPI.h"

#define IDC_ADDRESS_TXT 1000
#define IDC_MEMORY_TXT 1001

class MemoryToolBar;
class EditBtn;

///////////////////////////////////////////////////////////////////////////
class MemoryPanel
: public wxPanel
, public DocumentView
{
public:
	enum DisplayType
	{
		DST_HEX,
		DST_BINARY
	};

public:
	MemoryPanel(MemoryToolBar *toolBar, wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(819,191), long style = wxTAB_TRAVERSAL, const wxString &name = wxEmptyString);
	~MemoryPanel() override;

public: // DocumentView overrides
	wxWindow *getWindow(void) override { return this; }
	void activate(void) override;
	void deactivate(void) override;
	bool closing(bool forced, bool active) override;

public: // Control overrides
	void OnSize(wxSizeEvent &event);

public:
	void setAddress(size_t address);
	size_t getAddress(void) const { return m_address; }

	/**
	 * Text representation of the address. This is not neccessarily a
	 * hex value but might also be a formula or reference. This value
	 * is not evaluated, and serves only as a storage to keep the original
	 * representation local.
	 */
	void setAddressRep(const wxString &address);
	const wxString &getAddressRep(void) const { return m_addressRep; }

	/**
	 * Set the size of the memory block. If 0 it is unlimited
	 */
	void setMemorySize(uint32_t size);
	uint32_t getMemorySize(void) const { return m_memorySize; }

	void setColumns(uint32_t columns);
	uint32_t getColumns(void) const { return m_displayColumns; }

	void setBytes(uint32_t bytes);
	uint32_t getBytes(void) const { return m_bytes; }

	void setType(DisplayType type);
	DisplayType getType(void) const { return m_type; }

	void setSpaces(bool spaces);
	bool hasSpaces(void) const { return m_spaces; }

	void setLocked(bool locked);
	bool isLocked(void) const { return m_locked; }

protected:
	size_t printDump(size_t address);
	void OnAddressChanged(wxCommandEvent &event);
	void OnLock(wxCommandEvent &event);

private:
	void adjustDimensions(void);
	void Init(void);

private:
	EditBtn *m_addressTxt;
	wxTextCtrl *m_memoryTxt;
	wxFont m_font;
	MemoryToolBar *m_toolBar;

	size_t m_address;
	size_t m_curAddress;
	wxString m_addressRep;
	size_t m_memorySize;
	size_t m_displayColumns;
	size_t m_columns;
	size_t m_curLines;
	size_t m_bytes;
	DisplayType m_type;
	std::vector<DbgByte> m_lockCache;
	size_t m_addressLimit;

	bool m_spaces:1;
	bool m_locked:1;

	wxDECLARE_EVENT_TABLE();
};
