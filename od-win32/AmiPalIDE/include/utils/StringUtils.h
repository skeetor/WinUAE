#pragma once

#include <wx/confbase.h>
#include <string>
#include <cstdlib>

template <typename T>
wxString toHexString(T w, size_t len = sizeof(T) << 1)
{
	static const char *digits = "0123456789ABCDEF";
	wxString rc(len + 2, '0');
	rc[0] = '0';
	rc[1] = 'x';

	for (size_t i = 0, j = (len - 1) * 4; i < len; ++i, j -= 4)
		rc[i+2] = digits[((size_t)w >> j) & 0x0f];

	return rc;
}

template<typename T>
wxString toWxString(T value, bool hex = false)
{
	if (hex)
		return toHexString(value);

	return std::to_string(value).c_str();
}

static inline size_t fromWxString(const wxString value, int base = 10)
{
	return std::strtoull(value.c_str(), nullptr, base);
}

/**
 * Removes the next token from the string, and returns it.
 * If there are no more tokens, an empty string is returned.
 */
wxString getToken(wxString &typeInfo, wxString const &separator);
