#include "gui/DocumentWindow.h"
#include "gui/panels/DocumentPanel.h"

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

DocumentWindow *DocumentWindow::createFromInfo(wxWindow *parent, wxString typeInfo)
{
	wxString type;
	wxWindowID id = wxID_ANY;

	size_t pos = typeInfo.find('|');
	if (pos == wxString::npos)
		pos = typeInfo.length();

	type = typeInfo.substr(0, pos);

	if (pos < typeInfo.length())
	{
		typeInfo = typeInfo.substr(pos + 1);
		id = fromWxString(typeInfo, 16);
	}

	if (type == "DocumentPanel")
		return new DocumentPanel(parent, id);

	return nullptr;
}
