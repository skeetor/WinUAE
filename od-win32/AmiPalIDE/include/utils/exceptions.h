#pragma once

#include <stdexcept>

class SilentException
: public std::runtime_error
{
public:
	SilentException(const std::string &msg)
	: std::runtime_error(msg)
	{
	}
};
