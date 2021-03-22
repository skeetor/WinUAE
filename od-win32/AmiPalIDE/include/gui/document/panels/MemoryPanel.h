#pragma once

#include <wx/control.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

#include "gui/document/Document.h"
#include "patterns/observer.h"
#include "config/DebuggerConfig.h"

#define IDC_ADDRESS_TXT 1000
#define IDC_MEMORY_TXT 1001

class MemoryToolBar;
class EditBtn;
class ClDbgByte;

///////////////////////////////////////////////////////////////////////////
class MemoryPanel
: public wxPanel
, public Document
, Listener<DebuggerConfig>
{
public:
	enum DisplayType
	{
		DST_HEX,
		DST_BINARY
	};

public:
	static wxString displayTypeToString(DisplayType value);
	static DisplayType stringToDisplayType(const wxString &value);

public:
	MemoryPanel(MemoryToolBar *toolBar, wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(819,191), long style = wxTAB_TRAVERSAL, const wxString &name = wxEmptyString);
	~MemoryPanel() override;

public: // Document overrides
	wxWindow *getWindow(void) override { return this; }
	void activate(void) override;
	void deactivate(void) override;
	bool closing(bool forced, bool active) override;

	bool serialize(wxString groupId, wxConfigBase *config) override;
	bool deserialize(wxString groupId, wxConfigBase *config) override;

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

	void setColumnBytes(uint32_t bytes);
	uint32_t getColumnBytes(void) const { return m_columnBytes; }

	void setType(DisplayType type);
	DisplayType getType(void) const { return m_type; }

	void setSpaces(bool spaces);
	bool hasSpaces(void) const { return m_spaces; }

	void lock(bool locked) override;

protected:
	size_t printDump(size_t address);
	void OnAddressChanged(wxCommandEvent &event);
	void OnLock(wxCommandEvent &event);
	void handleNotification(DebuggerConfig &config);

private:
	void adjustDimensions(void);
	void Init(void);

private:
	EditBtn *m_addressTxt;
	wxTextCtrl *m_memoryTxt;
	MemoryToolBar *m_toolBar;
	uint32_t m_horizSB;
	uint32_t m_vertSB;

	size_t m_address;
	size_t m_curAddress;
	wxString m_addressRep;
	size_t m_memorySize;
	size_t m_displayColumns;
	size_t m_columns;
	size_t m_curLines;
	size_t m_columnBytes;
	DisplayType m_type;
	std::vector<ClDbgByte> m_lockCache;
	size_t m_addressLimit;

	bool m_spaces:1;
	bool m_visible:1;

	wxDECLARE_EVENT_TABLE();
};
