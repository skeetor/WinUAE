#pragma once

class DebuggerGuiView : public CEditView
{
protected:
	DebuggerGuiView();
	virtual ~DebuggerGuiView();

	DECLARE_DYNCREATE(DebuggerGuiView)

public:
	DebuggerGuiDoc* GetDocument() const;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	afx_msg void OnEditTogglebreakpoint();
	afx_msg void OnFind();
	afx_msg void OnFilePrintPreview();

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

private:
	void VerifyFindString(CMFCToolBarComboBoxButton* pFindCombo, CString& strFindText);

protected:
	DECLARE_MESSAGE_MAP()
};

inline DebuggerGuiDoc* DebuggerGuiView::GetDocument() const { return reinterpret_cast<DebuggerGuiDoc*>(m_pDocument); }
