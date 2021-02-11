#pragma once

class MemoryPane : public CDockablePane
{
public:
	MemoryPane();
	virtual ~MemoryPane();

protected:
	virtual BOOL CanAdjustLayout() const { return !m_bIsSliding || !m_bIsHiding; }

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()

protected:
	CFont m_Font;
	CMFCListCtrl m_wndWatch;
};
