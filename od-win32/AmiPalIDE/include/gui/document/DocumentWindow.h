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

	bool serialize(wxString groupId, wxConfigBase *config) override;
	bool deserialize(wxString groupId, wxConfigBase *config) override;

public:

	/**
	 * Returns a string which identfies the document type and must be
	 * unique for all documents.
	 */
	const wxString &getTypeInfo(void) const { return m_typeInfo; }

	const wxString &getTitle(void) const { return m_title; }
	void setTitle(const wxString &title) { m_title = title; }

	/**
	 * Factory method to create the document from a serialized typeinfo string.
	 */
	static DocumentWindow *createFromInfo(wxWindow *parent, wxString typeInfo);

protected:
	void setTypeInfo(const wxString &typeInfo) { m_typeInfo = typeInfo; }

private:
	wxString m_typeInfo;
	wxString m_title;
	wxString m_tooltip;
};

#define CREATE_DOCUMENT_WINDOW(className, parent) dynamic_cast<className *>(DocumentWindow::createFromInfo(parent, STRINGIFY(className)))
