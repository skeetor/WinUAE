#include "stdafx.h"
#include "app/DebuggerGui.h"
#include "app/MainFrame.h"
#include "ResourceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ResourceViewBar::ResourceViewBar()
{
}

ResourceViewBar::~ResourceViewBar()
{
}

BEGIN_MESSAGE_MAP(ResourceViewBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int ResourceViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndResourceView.Create(dwViewStyle, rectDummy, this, 3))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	m_ResourceViewImages.Create(IDB_RESOURCE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndResourceView.SetImageList(&m_ResourceViewImages, TVSIL_NORMAL);

	// Fill view context(dummy code, don't seek here something magic :-)):
	FillResourceView();

	OnChangeVisualStyle();
	return 0;
}

void ResourceViewBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (CanAdjustLayout())
	{
		m_wndResourceView.SetWindowPos(NULL, 1, 1, cx - 2, cy - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void ResourceViewBar::FillResourceView()
{
	HTREEITEM hRoot = m_wndResourceView.InsertItem(_T("Hello resources"), 0, 0);
	m_wndResourceView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hFolder = m_wndResourceView.InsertItem(_T("Accelerator"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("IDR_MAINFRAME"), 1, 1, hFolder);

	m_wndResourceView.Expand(hRoot, TVE_EXPAND);

	hFolder = m_wndResourceView.InsertItem(_T("Dialog"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("IDD_ABOUTBOX"), 3, 3, hFolder);

	hFolder = m_wndResourceView.InsertItem(_T("Icon"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("IDR_HELLO"), 4, 4, hFolder);
	m_wndResourceView.InsertItem(_T("IDR_MAINFRAME"), 4, 4, hFolder);

	m_wndResourceView.Expand(hFolder, TVE_EXPAND);

	hFolder = m_wndResourceView.InsertItem(_T("Menu"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("IDR_CONTEXT_MENU"), 5, 5, hFolder);
	m_wndResourceView.InsertItem(_T("IDR_HELLO"), 5, 5, hFolder);
	m_wndResourceView.InsertItem(_T("IDR_MAINFRAME"), 5, 5, hFolder);
	m_wndResourceView.InsertItem(_T("IDR_POPUP_TOOLBAR"), 5, 5, hFolder);

	m_wndResourceView.Expand(hFolder, TVE_EXPAND);

	hFolder = m_wndResourceView.InsertItem(_T("String Table"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("String Table"), 6, 6, hFolder);

	hFolder = m_wndResourceView.InsertItem(_T("Toolbar"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("IDR_MAINFRAME"), 7, 7, hFolder);

	hFolder = m_wndResourceView.InsertItem(_T("Version"), 0, 0, hRoot);
	m_wndResourceView.InsertItem(_T("VS_VERSION_INFO"), 8, 8, hFolder);
}

void ResourceViewBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndResourceView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
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
	DebuggerGuiApp::getInstance().GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_RESOURCE, point.x, point.y, this, TRUE);
}

void ResourceViewBar::OnEditCut()
{
}

void ResourceViewBar::OnEditCopy()
{
}

void ResourceViewBar::OnEditPaste()
{
}

void ResourceViewBar::OnEditClear()
{
}

void ResourceViewBar::OnPaint()
{
	CPaintDC dc(this);

	CRect rectTree;
	m_wndResourceView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void ResourceViewBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndResourceView.SetFocus();
}

void ResourceViewBar::OnChangeVisualStyle()
{
	DebuggerGuiApp &app = DebuggerGuiApp::getInstance();

	m_ResourceViewImages.DeleteImageList();

	UINT uiBmpId = app.m_bHiColorIcons ? IDB_RESOURCE_VIEW24 : IDB_RESOURCE_VIEW;

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

	m_ResourceViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ResourceViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndResourceView.SetImageList(&m_ResourceViewImages, TVSIL_NORMAL);
}
