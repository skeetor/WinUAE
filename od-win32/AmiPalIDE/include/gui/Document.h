#pragma once

#include "wx/window.h"

class Document
{
public:
	Document(void)
	: m_locked(false)
	{}
	virtual ~Document() {}

public:
	/**
	 * Get the window, associated with the document.
	 */
	virtual wxWindow *getWindow(void) = 0;

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
