#include "gui/DocumentManager.h"
#include <wx/confbase.h>
#include <string>

using namespace std;

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

	wxAuiPaneInfoArray const &pa = GetAllPanes();

	for (size_t i = 0; i < pa.Count(); i++)
	{
		wxAuiPaneInfo const &info = pa[i];
		DocumentWindow *d = reinterpret_cast<DocumentWindow *>(info.window->GetClientData());
		wxString id = groupId + "Pane_" + to_string(i);
		d->serialize(id + "_", config);
		wxString s = SavePaneInfo(info);
		config->Write(id, s);
	}

	config->Write(groupId + "PerspectiveLayout", SavePerspective());

	return true;
}

bool DocumentManager::deserialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/Documents");

	LoadPerspective(config->Read(groupId + "PerspectiveLayout", ""));

	return true;
}
