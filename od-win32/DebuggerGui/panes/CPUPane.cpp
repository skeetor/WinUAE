#include "stdafx.h"
#include "CPUPane.h"

#include "app/DebuggerGui.h"
#include "debugger/DebuggerAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_LIST_1 1

CPUPane::CPUPane()
{
}

CPUPane::~CPUPane()
{
}

BEGIN_MESSAGE_MAP(CPUPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int CPUPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT;

	m_wndWatch.Create(dwStyle, rectDummy, this, ID_LIST_1);
	m_wndWatch.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_wndWatch.InsertColumn(0, _T("D0"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn(1, _T("D1"), LVCFMT_LEFT, 100);

	m_wndWatch.InsertItem(0, _T("m_nCount"));
	m_wndWatch.SetItemText(0, 1, _T("100"));

	m_wndWatch.SetFont(&m_Font);

	return 0;
}

void CPUPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (CanAdjustLayout())
	{
		CRect rc;
		GetClientRect(rc);

		m_wndWatch.SetWindowPos(NULL, rc.left + 1, rc.top + 1, rc.Width() - 2, rc.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPUPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rectTree;
	m_wndWatch.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPUPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndWatch.SetFocus();
}

