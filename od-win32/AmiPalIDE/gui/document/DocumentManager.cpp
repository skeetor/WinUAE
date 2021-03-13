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

	config->Write(groupId + "Layout", SavePerspective());

	wxAuiPaneInfoArray const &pa = GetAllPanes();

	for (size_t i = 0; i < pa.Count(); i++)
	{
		wxAuiPaneInfo const &info = pa[i];
		DocumentWindow *d = reinterpret_cast<DocumentWindow *>(info.window->GetClientData());
		wxString id = groupId + "Pane_" + to_string(i) + "_";

		config->Write(id + "Type", d->getTypeInfo());
		config->Write(id + "Layout", SavePaneInfo(info));

		d->serialize(id, config);
	}

	return true;
}

bool DocumentManager::deserialize(wxString const &groupId, wxConfigBase *config)
{
	config->SetPath("/Documents");

	size_t i = 0;
	wxString v;
	wxString id;

	while ((v = config->Read((id = groupId + "Pane_" + to_string(i++) + "_") + "Type", "")) != "")
	{
		DocumentWindow *d = DocumentWindow::createFromInfo(GetManagedWindow(), v);
		checkException(!d, "Unknown type: ", id + "Type", v);

		checkException((v = config->Read(id + "Layout", "")).empty(), "", id + "Layout", v);
		wxAuiPaneInfo info;
		LoadPaneInfo(v, info);

		d->deserialize(id, config);
		AddPane(d, info);
	}

	LoadPerspective(config->Read(groupId + "Layout", ""));

	return true;
}
