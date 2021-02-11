#pragma once

class ClassTreeWnd : public CTreeCtrl
{
public:
	ClassTreeWnd();
	virtual ~ClassTreeWnd();

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
};
