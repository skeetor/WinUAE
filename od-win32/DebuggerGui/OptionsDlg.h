#pragma once

#include "OptionsPages.h"

class OptionsDlg : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(OptionsDlg)

public:
	OptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~OptionsDlg();

public:
	OptionsPage1  m_Page11;
	OptionsPage12 m_Page12;
	OptionsPage21 m_Page21;
	OptionsPage22 m_Page22;
	OptionsPage31 m_Page31;
	OptionsPage32 m_Page32;

protected:
	DECLARE_MESSAGE_MAP()
};

