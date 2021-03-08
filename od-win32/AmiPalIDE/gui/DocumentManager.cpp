#include "gui/DocumentManager.h"
#include <wx/confbase.h>

DocumentManager::DocumentManager(wxWindow *managedWnd, unsigned int flags)
: wxAuiManager(managedWnd, flags)
{
}

DocumentManager::~DocumentManager()
{
}

bool DocumentManager::AddPane(DocumentWindow *document, const wxAuiPaneInfo &paneInfo)
{
	return wxAuiManager::AddPane(document->getWindow(), paneInfo);
}

bool DocumentManager::AddPane(DocumentWindow *document, const wxAuiPaneInfo &paneInfo, const wxPoint &dropPos)
{
	return wxAuiManager::AddPane(document->getWindow(), paneInfo, dropPos);
}

bool DocumentManager::AddPane(DocumentWindow *document, int direction, const wxString &caption)
{
	return wxAuiManager::AddPane(document->getWindow(), direction, caption);
}

bool DocumentManager::InsertPane(DocumentWindow *document, const wxAuiPaneInfo &insertLocation, int insertLevel)
{
	return wxAuiManager::InsertPane(document->getWindow(), insertLocation, insertLevel);
}

bool DocumentManager::serialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/Documents");

	config->Write("PerspectiveLayout", SavePerspective());

	return true;
}

bool DocumentManager::deserialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/Documents");

	LoadPerspective(config->Read("PerspectiveLayout", ""));

	return true;
}
