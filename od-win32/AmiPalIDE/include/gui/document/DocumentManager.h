#pragma once

#include <wx/aui/aui.h>

#include "gui/document/Document.h"

///////////////////////////////////////////////////////////////////////////
class DocumentManager
: public wxAuiManager
, public Serialize
{
public:
	DocumentManager(wxWindow *managedWnd = NULL, unsigned int flags = wxAUI_MGR_DEFAULT);
	~DocumentManager() override;

public:
	bool AddPane(DocumentWindow *document, const wxAuiPaneInfo &paneInfo);
	bool AddPane(DocumentWindow *document, const wxAuiPaneInfo &paneInfo, const wxPoint &dropPos);
	bool AddPane(DocumentWindow *document, int direction = wxLEFT, const wxString &caption = wxEmptyString);
	bool InsertPane(DocumentWindow *document, const wxAuiPaneInfo &insertLocation, int insertLevel = wxAUI_INSERT_PANE);

	bool serialize(wxString const &groupId, wxConfigBase *config) override;
	bool deserialize(wxString const &groupId, wxConfigBase *config) override;
};
