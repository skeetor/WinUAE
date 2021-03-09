#pragma once

#include "patterns/serialize.h"

#include <wx/window.h>

class DocumentWindow
: public Serialize
{
public:
	DocumentWindow(wxString const &typeName, wxWindow *window = nullptr)
	: m_typeName(typeName)
	{
		if (window)
			window->SetClientData(this);
	}

	virtual ~DocumentWindow() {}

public:
	/**
	 * Get the window, associated with the document.
	 */
	virtual wxWindow *getWindow(void) = 0;

	/**
	 * Returns a string which identfies the document type and must be
	 * unique for all documents.
	 */
	const wxString &getTypeName(void) { return m_typeName; }

private:
	wxString m_typeName;
};
