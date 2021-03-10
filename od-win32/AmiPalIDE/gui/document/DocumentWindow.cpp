#include "AmiPalIDE.h"

#include "gui/MainFrame.h"
#include "gui/document/DocumentWindow.h"
#include "gui/document/panels/DocumentPanel.h"
#include "gui/document/panels/MemoryPanel.h"
#include "gui/document/MemoryToolBar.h"
#include "gui/document/FileTree.h"

#include "utils/StringUtils.h"

DocumentWindow::DocumentWindow(wxString const &typeInfo, wxWindow *window)
: m_typeInfo(typeInfo)
{
	if (window)
		window->SetClientData(this);
}

DocumentWindow::~DocumentWindow(void)
{
}

wxString DocumentWindow::getInfoToken(wxString &typeInfo)
{
	size_t pos = typeInfo.find('|');
	wxString token;

	if (pos == wxString::npos)
	{
		token = typeInfo;
		typeInfo = "";
		return token;
	}

	token = typeInfo.substr(0, pos);
	typeInfo = typeInfo.substr(pos + 1);

	return token;
}

DocumentWindow *DocumentWindow::createFromInfo(wxWindow *parent, wxString typeInfo)
{
	wxWindowID id = wxID_ANY;
	MainFrame *frame = wxGetApp().m_mainFrame;
	wxString type = getInfoToken(typeInfo);

	if (!typeInfo.empty())
		id = fromWxString(getInfoToken(typeInfo), 16);

	if (type == "DocumentPanel")
		return new DocumentPanel(parent, id);

	if (type == "MemoryToolBar")
		return new MemoryToolBar(frame, parent, id);

	if (type == "FileTree")
		return new FileTree(parent, id);

	return nullptr;
}

Document *Document::createDocumentFromInfo(wxWindow *parent, wxString typeInfo)
{
	wxWindowID id = wxID_ANY;
	MainFrame *frame = wxGetApp().m_mainFrame;

	wxString type = getInfoToken(typeInfo);

	if (type == "MemoryPanel")
		return new MemoryPanel(frame->getMemoryToolBar(), parent, id);

	return nullptr;
}
