#pragma once

class OutputList1 : public CListBox
{
public:
	OutputList1();
	virtual ~OutputList1();

protected:
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnOutputGotoError();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class OutputBar : public CDockablePane
{
public:
	OutputBar();
	virtual ~OutputBar();

public:
	void SetFlatTabs(BOOL bFlat = TRUE, BOOL bRepaint = TRUE)
	{
		m_wndTabs.SetFlatFrame(bFlat, bRepaint);
	}

protected:
	void FillBuildWindow();
	void FillDebugWindow();
	void AdjusrHorzScroll(CListBox& wndListBox);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

protected:
	CMFCTabCtrl m_wndTabs;
	CFont m_Font;

	OutputList1 m_wndOutputBuild;
	OutputList1 m_wndOutputDebug;
	OutputList1 m_wndOutputFind1;
	OutputList1 m_wndOutputFind2;
	OutputList1 m_wndOutputSQL;
};


