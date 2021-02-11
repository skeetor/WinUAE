#pragma once

static CString g_strStartViewName = _T("mslocal://StartPage");

class ChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(ChildFrame)

public:
	ChildFrame();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);

	virtual BOOL IsReadOnly();
	virtual LPCTSTR GetDocumentName(CObject** pObj);

public:
	virtual ~ChildFrame();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};
