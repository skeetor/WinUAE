#pragma once

#include "controls/UndoListBox.h"

class UndoButton;

class UndoBar : public CMFCPopupMenuBar
{
	DECLARE_SERIAL(UndoBar)

public:
	UndoBar();
	virtual ~UndoBar();

public:
	void DoUndo();
	void SetLabel(const CString& strLabel);

	virtual void OnFillBackground(CDC* pDC);
	virtual CSize CalcSize(BOOL bVertDock);

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

	UndoButton* GetParentButton() const;

protected:
	UndoListBox m_wndList;
	CRect m_rectLabel;
	int m_nLabelHeight;
	CString m_strLabel;
};

//////////////////////////////////////////////////////////////////////////////
// UndoMenu

class UndoMenu : public CMFCPopupMenu
{
	DECLARE_SERIAL(UndoMenu)

	virtual CMFCPopupMenuBar* GetMenuBar()
	{
		return &m_wndUndoBar;
	}

	UndoBar m_wndUndoBar;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////
// UndoButton

class UndoButton : public CMFCToolBarMenuButton
{
	friend class UndoBar;

	DECLARE_SERIAL(UndoButton)

public:
	UndoButton()
	: m_nSelNum(0)
	{
	}

	UndoButton(UINT uiCmdID, LPCTSTR lpszText)
	: CMFCToolBarMenuButton(uiCmdID, NULL, GetCmdMgr()->GetCmdImage(uiCmdID, FALSE), lpszText)
	, m_nSelNum(0)
	{
	}

	int GetSelNum()
	{
		return m_nSelNum;
	}

	void ResetSelNum()
	{
		m_nSelNum = 0;
	}

protected:
	virtual CMFCPopupMenu* CreatePopupMenu();

	virtual BOOL IsEmptyMenuAllowed() const
	{
		return TRUE;
	}

	virtual void OnChangeParentWnd(CWnd* pWndParent)
	{
		CMFCToolBarMenuButton::OnChangeParentWnd(pWndParent);
		m_bDrawDownArrow = TRUE;
	}

	virtual BOOL IsExclusive() const
	{
		return TRUE;
	}

public:
	CStringList m_lstActions;
	int m_nSelNum;
};
