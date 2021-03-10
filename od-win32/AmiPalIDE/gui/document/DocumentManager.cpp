#include "gui/document/DocumentManager.h"

#include <wx/confbase.h>

#include <string>
#include <stdexcept>

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

		config->Write(id + "_Type", d->getTypeInfo());
		config->Write(id + "_TypeInfo", SavePaneInfo(info));
		d->serialize(id + "_", config);
	}

	config->Write(groupId + "layout", SavePerspective());

	return true;
}

bool DocumentManager::deserialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/Documents");

	size_t i = 0;
	wxString v;
	wxString id = groupId + "Pane_" + to_string(i);

	while ((v = config->Read(id + "_Type", "")) != "")
	{
		DocumentWindow *d = DocumentWindow::createFromInfo(GetManagedWindow(), v);

		checkException(!d, "Unknown type: ", id + "_Type", v);
		checkException((v = config->Read(id + "_TypeInfo", "")).empty(), "", id + "_TypeInfo", v);

		wxAuiPaneInfo info;
		LoadPaneInfo(v, info);

		d->deserialize(id + "_", config);
		AddPane(d, info);

		i++;
		id = groupId + "Pane_" + to_string(i);
	}

	LoadPerspective(config->Read(groupId + "layout", ""));

	return true;
}
