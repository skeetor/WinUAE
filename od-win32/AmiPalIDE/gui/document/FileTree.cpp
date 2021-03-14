#include "gui/document/FileTree.h"

#include <wx/imaglist.h>
#include <wx/artprov.h>

FileTree::FileTree(wxWindow *parent, wxWindowID id, wxSize size)
: wxTreeCtrl(parent, id, wxPoint(0, 0), size, wxTR_DEFAULT_STYLE | wxNO_BORDER)
, DocumentWindow("FileTree", this)
{
	size = FromDIP(wxSize(16, 16));
	wxImageList *imglist = new wxImageList(size.x, size.y, true, 2);
	imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, size));
	imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, size));
	AssignImageList(imglist);

	wxTreeItemId root = AddRoot("wxAUI Project", 0);
	wxArrayTreeItemIds items;

	items.Add(AppendItem(root, "Item 1", 0));
	items.Add(AppendItem(root, "Item 2", 0));
	items.Add(AppendItem(root, "Item 3", 0));
	items.Add(AppendItem(root, "Item 4", 0));
	items.Add(AppendItem(root, "Item 5", 0));

	int i, count;
	for (i = 0, count = items.Count(); i < count; ++i)
	{
		wxTreeItemId id = items.Item(i);
		AppendItem(id, "Subitem 1", 1);
		AppendItem(id, "Subitem 2", 1);
		AppendItem(id, "Subitem 3", 1);
		AppendItem(id, "Subitem 4", 1);
		AppendItem(id, "Subitem 5", 1);
	}

	Expand(root);
}

FileTree::~FileTree(void)
{
}

bool FileTree::serialize(wxString groupId, wxConfigBase *config)
{
	return DocumentWindow::serialize(groupId, config);
}

bool FileTree::deserialize(wxString groupId, wxConfigBase *config)
{
	return DocumentWindow::deserialize(groupId, config);
}

