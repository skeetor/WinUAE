#pragma once

#include <wx/string.h>

class wxConfigBase;

class Serialize
{
public:
	Serialize(void) = default;
	~Serialize(void) = default;

	virtual bool serialize(wxString const &groupId, wxConfigBase *config) = 0;
	virtual bool deserialize(wxString const &groupId, wxConfigBase *config) = 0;
};
