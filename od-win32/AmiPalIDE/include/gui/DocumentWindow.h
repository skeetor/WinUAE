#pragma once

#include "patterns/serialize.h"

#include <wx/window.h>

class DocumentWindow
: public Serialize
{
public:
	DocumentWindow(wxString const &typeInfo, wxWindow *window = nullptr);
	virtual ~DocumentWindow(void);

public:
	/**
	 * Get the window, associated with the document.
	 */
	virtual wxWindow *getWindow(void) = 0;

public:
	/**
	 * Factory method to create the document from a serialized typeinfo string.
	 */
	static DocumentWindow *createFromInfo(wxWindow *parent, wxString typeInfo);

	/**
	 * Returns a string which identfies the document type and must be
	 * unique for all documents.
	 */
	const wxString &getTypeInfo(void) { return m_typeInfo; }

protected:
	void setTypeInfo(const wxString &typeInfo) { m_typeInfo = typeInfo; }

private:
	wxString m_typeInfo;
};
