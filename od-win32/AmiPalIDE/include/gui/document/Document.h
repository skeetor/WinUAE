#pragma once

#include "gui/document/DocumentWindow.h"

class Document
: public DocumentWindow
{
	friend class DocumentManager;
	friend class DocumentPanel;

public:
	Document(wxString const &typeInfo, wxWindow *window = nullptr)
	: DocumentWindow(typeInfo)
	, m_locked(false)
	{
		if (window)
			window->SetClientData(this);
	}

	~Document() override {}

public:
	/**
	 * Called when the document is made the active one.
	 */
	virtual void activate(void) = 0;

	/**
	 * Called when the document is no longer active.
	 */
	virtual void deactivate(void) = 0;

	/**
	 * Called when the document is about to be closed. If the document
	 * can not be closed it should return false. This is ignored if
	 * 'force'  is true. In that case the document has to be closed anyway.
	 * 
	 * 'active' is true, if the document was currently the active one.
	 */
	virtual bool closing(bool forced, bool active) = 0;

	/**
	 * Lock the document, so it is not modified anymore.
	 */
	virtual void lock(bool state) { m_locked = state; }
	virtual bool isLocked(void) { return m_locked; }


	static Document *createFromInfo(wxWindow *parent, wxString typeInfo);

private:
	bool m_locked:1;
};

#define CREATE_DOCUMENT(className, parent) dynamic_cast<className *>(Document::createFromInfo(parent, STRINGIFY(className)))
