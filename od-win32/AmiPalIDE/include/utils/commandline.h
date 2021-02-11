#pragma once

#include <vector>
#include <string>

int findParam(int argc, wchar_t **argv, const wchar_t *name, std::vector<std::wstring> &params, int start = 1);

