#pragma once

#include "patterns/serialize.h"

#include <wx/window.h>

class DocumentWindow
: public Serialize
{
public:
	DocumentWindow(void) {}
	virtual ~DocumentWindow() {}

public:
	/**
	 * Get the window, associated with the document.
	 */
	virtual wxWindow *getWindow(void) = 0;
};
