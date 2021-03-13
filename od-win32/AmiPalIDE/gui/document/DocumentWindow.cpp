#include "gui/MainFrame.h"
#include "gui/document/DocumentWindow.h"
#include "gui/document/panels/DocumentPanel.h"
#include "gui/document/panels/MemoryPanel.h"
#include "gui/document/MemoryToolBar.h"
#include "gui/document/FileTree.h"

#include "utils/StringUtils.h"

#include <wx/msgdlg.h>

DocumentWindow::DocumentWindow(wxString const &typeInfo, wxWindow *window)
: m_typeInfo(typeInfo)
{
	if (window)
		window->SetClientData(this);
}

DocumentWindow::~DocumentWindow(void)
{
}

DocumentWindow *DocumentWindow::createFromInfo(wxWindow *parent, wxString typeInfo)
{
	wxWindowID id = wxID_ANY;
	MainFrame *frame = MainFrame::getInstance();
	wxString type = getToken(typeInfo, "|");

	if (!typeInfo.empty())
		id = fromWxString(getToken(typeInfo, "|"), 16);

	if (type == "DocumentPanel")
		return new DocumentPanel(parent, id);

	if (type == "MemoryToolBar")
		return new MemoryToolBar(frame, parent, id);

	if (type == "FileTree")
		return new FileTree(parent, id);

	return nullptr;
}

Document *Document::createFromInfo(wxWindow *parent, wxString typeInfo)
{
	wxWindowID id = wxID_ANY;
	MainFrame *frame = MainFrame::getInstance();

	wxString type = getToken(typeInfo, "|");

	if (type == "MemoryPanel")
		return new MemoryPanel(frame->getMemoryToolBar(), parent, id);

	return nullptr;
}
