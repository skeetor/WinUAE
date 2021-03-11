#pragma once

#include "patterns/serialize.h"
#include "framework.h"

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
	 * Returns a string which identfies the document type and must be
	 * unique for all documents.
	 */
	const wxString &getTypeInfo(void) { return m_typeInfo; }

	/**
	 * Factory method to create the document from a serialized typeinfo string.
	 */
	static DocumentWindow *createFromInfo(wxWindow *parent, wxString typeInfo);

protected:
	void setTypeInfo(const wxString &typeInfo) { m_typeInfo = typeInfo; }

	/**
	 * Removes the next token from the info string, and returns it.
	 * If there are no more tokens, an empty string is returned.
	 */
	static wxString getInfoToken(wxString &typeInfo);

private:
	wxString m_typeInfo;
};

#define CREATE_DOCUMENT_WINDOW(className, parent) dynamic_cast<className *>(DocumentWindow::createFromInfo(parent, STRINGIFY(className)))
