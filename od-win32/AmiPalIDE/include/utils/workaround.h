#pragma once

#include <wx/confbase.h>
#include <string>
#include <cstdlib>

// Workaround because wxConfBase/wxToString lacks an overload for uin64_t
static inline wxString toWxString(uint64_t value)
{
	return std::to_string(value).c_str();
}

static inline size_t fromWxString(const wxString value, int base = 10)
{
	return std::strtoull(value.c_str(), nullptr, base);
}
