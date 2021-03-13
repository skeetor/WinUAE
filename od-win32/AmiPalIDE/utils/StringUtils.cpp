#include "utils/StringUtils.h"

wxString getToken(wxString &typeInfo, wxString const &separator)
{
	size_t pos = typeInfo.find(separator);
	wxString token;

	if (pos == wxString::npos)
	{
		token = typeInfo;
		typeInfo = "";
		return token;
	}

	token = typeInfo.substr(0, pos);
	typeInfo = typeInfo.substr(pos + separator.length());

	return token;
}
