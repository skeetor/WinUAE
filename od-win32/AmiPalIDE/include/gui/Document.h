#pragma once

#include "gui/DocumentWindow.h"

class Document
: public DocumentWindow
{
public:
	Document(wxString const &typeName, wxWindow *window = nullptr)
	: DocumentWindow(typeName)
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

private:
	bool m_locked:1;
};
