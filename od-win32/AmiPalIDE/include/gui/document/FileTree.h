#pragma once

#include "gui/document/DocumentWindow.h"

#include <wx/treectrl.h>

class FileTree
: public wxTreeCtrl
, public DocumentWindow
{
public:
	FileTree(wxWindow *parent, wxWindowID id = wxID_ANY, wxSize size = wxDefaultSize);
	~FileTree(void) override;

	wxWindow *getWindow(void) override { return this; }

	bool serialize(wxString groupId, wxConfigBase *config) override;
	bool deserialize(wxString groupId, wxConfigBase *config) override;
};
