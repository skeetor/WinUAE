#pragma once

#include <stdexcept>

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

class SerializeException
: public std::runtime_error
{
public:
	SerializeException(const std::string &msg, const std::string &key, const std::string &value)
	: std::runtime_error(msg)
	, m_key(key)
	, m_value(value)
	{
	}

	const std::string &getKey(void) const { return m_key; }
	const std::string &getValue(void) const { return m_value; }

private:
	std::string m_key;
	std::string m_value;
};

void checkException(bool Throw, const wxString &msg, const wxString &key, const wxString &value);
