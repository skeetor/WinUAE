#pragma once

class StartView : public CView
{
protected:
	StartView();           // protected constructor used by dynamic creation
	virtual ~StartView();

	DECLARE_DYNCREATE(StartView)

public:
	virtual void OnInitialUpdate();

protected:
	virtual void OnDraw(CDC* pDC);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDisableUpdate(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	CMFCToolBarImages m_Image;
};
