
#include "utils/commandline.h"
#include <cstring>

using namespace std;

int findParam(int argc, wchar_t **argv, const wchar_t *n, vector<wstring> &params, int start)
{
	params.clear();
	wstring name(n);

	char par = false;
	size_t nameLen = name.length();
	int i = 0;

	// In wxWidgets the last argv is not NULL but an empty string
	for (i = start; i < argc-1; i++)
	{
		wstring p = argv[i];
		size_t pos = 0;

		if (!par)
		{
			if (p.length() <= 2)
			{
				if (nameLen > 2)
					continue;
			}

			if (p[pos] == '-')
				pos++;

			if (p[pos] == '-')
				pos++;

			if (name == &p[pos])
				par = true;

			continue;
		}

		if (p[0] == '-')
			break;

		params.push_back(p);
	}

	if (!par)
		return -1;

	return i;
}
