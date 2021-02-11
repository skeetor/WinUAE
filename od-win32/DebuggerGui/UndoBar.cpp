#include "stdafx.h"
#include "UndoBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(UndoBar, CMFCPopupMenuBar, 1)

const int ID_LIST = 1;

#pragma warning(disable: 4355)
UndoBar::UndoBar() : m_wndList(*this), m_nLabelHeight(0)
#pragma warning(default: 4355)
{
	m_rectLabel.SetRectEmpty();
}

UndoBar::~UndoBar()
{
}

BEGIN_MESSAGE_MAP(UndoBar, CMFCPopupMenuBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

void UndoBar::OnFillBackground(CDC* pDC)
{
	ASSERT_VALID(pDC);

	CRect rectClient;
	GetClientRect(rectClient);

	::FillRect(pDC->GetSafeHdc(), &rectClient, ::GetSysColorBrush(COLOR_3DFACE));

	pDC->Draw3dRect(m_rectLabel, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DHILIGHT));

	CFont* pOldFont = pDC->SelectObject((CFont*) &CMFCMenuBar::GetMenuFont());
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));

	CRect rectText = m_rectLabel;
	rectText.DeflateRect(1, 1);
	pDC->DrawText(m_strLabel, rectText, DT_CENTER | DT_VCENTER);

	pDC->SelectObject(pOldFont);
}

CSize UndoBar::CalcSize(BOOL /*bVertDock*/)
{
	return CSize(160, 115);
}

UndoButton* UndoBar::GetParentButton() const
{
	UndoButton* pUndoButton = NULL;

	UndoMenu* pParentMenu = DYNAMIC_DOWNCAST(UndoMenu, GetParent());
	if (pParentMenu != NULL)
	{
		pUndoButton = DYNAMIC_DOWNCAST(UndoButton, pParentMenu->GetParentButton());
	}

	return pUndoButton;
}

void UndoBar::OnSize(UINT nType, int cx, int cy)
{
	const int nMargin = 5;

	CMFCPopupMenuBar::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect(rectClient);

	rectClient.DeflateRect(nMargin, nMargin);

	m_rectLabel = rectClient;
	m_rectLabel.top = m_rectLabel.bottom - m_nLabelHeight;

	CRect rectList = rectClient;
	rectList.bottom = m_rectLabel.top - nMargin;

	m_wndList.MoveWindow(rectList);
}

int UndoBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPopupMenuBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont* pMenuFont = (CFont*) &CMFCMenuBar::GetMenuFont();

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(pMenuFont);

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	m_nLabelHeight = tm.tmHeight + 2;

	dc.SelectObject(pOldFont);

	CRect rectDummy(0, 0, 0, 0);
	m_wndList.Create(WS_VISIBLE | WS_CHILD | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_VSCROLL | LBS_MULTIPLESEL, rectDummy, this, ID_LIST);
	m_wndList.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	m_wndList.SetFont(pMenuFont);

	UndoButton* pUndoButton = GetParentButton();
	ASSERT_VALID(pUndoButton);

	for (POSITION pos = pUndoButton->m_lstActions.GetHeadPosition(); pos != NULL;)
	{
		m_wndList.AddString(pUndoButton->m_lstActions.GetNext(pos));
	}

	return 0;
}

void UndoBar::DoUndo()
{
	UndoButton* pUndoButton = GetParentButton();
	ASSERT_VALID(pUndoButton);

	pUndoButton->m_nSelNum = m_wndList.GetSelCount() + 1;

	CMDIChildWnd* pChildFrame = ((CMDIFrameWndEx*)GetTopLevelFrame())->MDIGetActive();
	if (pChildFrame)
	{
		CEditView* pView = DYNAMIC_DOWNCAST(CEditView, pChildFrame->GetActiveView());
		if (pView != NULL)
		{
		}
		else
		{
		}
	}
	else
	{
	}

	GetParent()->PostMessage(WM_CLOSE);
}

void UndoBar::SetLabel(const CString& strLabel)
{
	m_strLabel = strLabel;

	if (GetSafeHwnd() != NULL)
	{
		InvalidateRect(m_rectLabel);
		UpdateWindow();
	}
}

IMPLEMENT_SERIAL(UndoButton, CMFCToolBarMenuButton, 1)

CMFCPopupMenu* UndoButton::CreatePopupMenu()
{
	if (CMFCToolBar::IsCustomizeMode() ||(m_nStyle & TBBS_DISABLED))
	{
		return NULL;
	}

	UndoMenu* pPopupMenu = new UndoMenu;

	ResetSelNum();

#if 0
	CMDIChildWnd* pChildFrame = ((CMDIFrameWndEx*)::AfxGetMainWnd())->MDIGetActive();
	if (pChildFrame)
	{
		CDebuggerGuiView* pView = (CDebuggerGuiView*)pChildFrame->GetActiveView();

		if (pView && pView->IsKindOf(RUNTIME_CLASS(CDebuggerGuiView)))
		{
			CDWordArray dwaUAT;

			switch (m_nID)
			{
			case ID_EDIT_UNDO:
				pView->GetUndoActions(m_lstActions);

				break;
			case ID_EDIT_REDO:
				pView->GetRedoActions(m_lstActions);

				break;
			default:
				break;
			}
		}
		else
		{
			ASSERT(FALSE);
			m_lstActions.RemoveAll();
		}
	}
	else
	{
		ASSERT(FALSE);
		m_lstActions.RemoveAll();
	}
#endif

	return pPopupMenu;
}

IMPLEMENT_SERIAL(UndoMenu, CMFCPopupMenu, 1)

BEGIN_MESSAGE_MAP(UndoMenu, CMFCPopupMenu)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int UndoMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD toolbarStyle = AFX_DEFAULT_TOOLBAR_STYLE;
	if (m_AnimationType != NO_ANIMATION && !CMFCToolBar::IsCustomizeMode())
	{
		toolbarStyle &= ~WS_VISIBLE;
	}

	if (!m_wndUndoBar.Create(this, toolbarStyle | CBRS_TOOLTIPS | CBRS_FLYBY, 1))
	{
		TRACE(_T("Can't create popup menu bar\n"));
		return -1;
	}

	CWnd* pWndParent = GetParent();
	ASSERT_VALID(pWndParent);

	m_wndUndoBar.SetOwner(pWndParent);
	m_wndUndoBar.SetPaneStyle(m_wndUndoBar.GetPaneStyle() | CBRS_TOOLTIPS);

	ActivatePopupMenu(GetTopLevelFrame(), this);
	RecalcLayout();

	return 0;
}
