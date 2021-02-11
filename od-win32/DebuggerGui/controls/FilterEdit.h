#pragma once

#include <string>

#include "stdafx.h"

class FilterEdit
: public CEdit
{
public:
	FilterEdit(const TSTRING &allowedCharacters);
	~FilterEdit() override;

	const TSTRING &getAllowedCharacters(void) const { return m_allowedCharacters;  }
	void setAllowedCharacters(const TSTRING &allowedCharacters);

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP();

	virtual bool validateChar(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
	TSTRING m_allowedCharacters;
};
