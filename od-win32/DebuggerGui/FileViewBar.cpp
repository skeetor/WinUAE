#include "stdafx.h"
#include "app/DebuggerGui.h"
#include "app/MainFrame.h"
#include "FileViewBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

FileViewBar::FileViewBar()
{
}

FileViewBar::~FileViewBar()
{
}

BEGIN_MESSAGE_MAP(FileViewBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_SOLUTION_OPEN, OnSolutionOpen)
	ON_COMMAND(ID_SOLUTION_OPEN_WITH, OnSolutionOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int FileViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create solution explorer\n");
		return -1;
	}

	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SOLUTION_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_SOLUTION_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	FillFileView();
	AdjustLayout();

	return 0;
}

void FileViewBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void FileViewBar::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("Hello files"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("Source Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("Hello.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("Hello.rc"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("HelloDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("HelloView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("Header Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("Hello.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("HelloDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("HelloView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("Resource Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("Hello.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("Hello.rc2"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("HelloDoc.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("Toolbar.bmp"), 2, 2, hRes);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);
}

void FileViewBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	DebuggerGuiApp::getInstance().GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_SOLUTION, point.x, point.y, this, TRUE);
}

void FileViewBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void FileViewBar::OnProperties()
{
	AfxMessageBox(_T("Properties..."));

}

void FileViewBar::OnSolutionOpen()
{
}

void FileViewBar::OnSolutionOpenWith()
{
}

void FileViewBar::OnDummyCompile()
{
}

void FileViewBar::OnEditCut()
{
}

void FileViewBar::OnEditCopy()
{
}

void FileViewBar::OnEditClear()
{
}

void FileViewBar::OnPaint()
{
	CPaintDC dc(this);

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void FileViewBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void FileViewBar::OnChangeVisualStyle()
{
	DebuggerGuiApp &app = DebuggerGuiApp::getInstance();

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(app.m_bHiColorIcons ? IDB_SOLUTION_EXPLORER_24 : IDR_SOLUTION_EXPLORER, 0, 0, TRUE /* Locked */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = app.m_bHiColorIcons ? IDB_FILE_VIEW24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (app.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}
