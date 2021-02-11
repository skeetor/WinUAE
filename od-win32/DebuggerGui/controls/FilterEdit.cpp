#include <algorithm>

#include "FilterEdit.h"

using namespace std;

BEGIN_MESSAGE_MAP(FilterEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

FilterEdit::FilterEdit(const TSTRING &allowedCharacters)
{
	setAllowedCharacters(allowedCharacters);
}

FilterEdit::~FilterEdit()
{
}

void FilterEdit::setAllowedCharacters(const TSTRING &allowedCharacters)
{
	m_allowedCharacters = allowedCharacters;

	transform(m_allowedCharacters.begin(), m_allowedCharacters.end(), m_allowedCharacters.begin(),
		[](TCHAR c)
		{
			return tolower(c);
		}
	);
}

void FilterEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (validateChar(nChar, nRepCnt, nFlags))
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}

bool FilterEdit::validateChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_allowedCharacters.empty() && m_allowedCharacters.find(tolower(nChar)) == TSTRING::npos)
		return false;

	return true;
}
