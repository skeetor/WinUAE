#pragma once

class OutputList : public CListBox
{
public:
	OutputList();
	virtual ~OutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnOutputGotoError();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class OutputViewBar : public CDockablePane
{
public:
	OutputViewBar();
	virtual ~OutputViewBar();

	void UpdateFonts();

protected:
	void FillDebugWindow();
	void FillBuildWindow();
	void AdjusrHorzScroll(CListBox& wndListBox);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()

protected:
	OutputList m_wndOutput;
};
