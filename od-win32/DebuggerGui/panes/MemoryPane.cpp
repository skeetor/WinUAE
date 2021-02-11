#include <string>

#include "stdafx.h"
#include "MemoryPane.h"

#include "app/DebuggerGui.h"
#include "debugger/DebuggerAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_LIST_1 1

MemoryPane::MemoryPane()
{
}

MemoryPane::~MemoryPane()
{
}

BEGIN_MESSAGE_MAP(MemoryPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int MemoryPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	m_wndWatch.InsertItem(0, _T("0x00000000"));
	m_wndWatch.SetItemText(0, 1, _T(""));

	m_wndWatch.SetFont(&m_Font);

	return 0;
}

void MemoryPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (CanAdjustLayout())
	{
		CRect rc;
		GetClientRect(rc);

		m_wndWatch.SetWindowPos(NULL, rc.left + 1, rc.top + 1, rc.Width() - 2, rc.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void MemoryPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rectTree;
	m_wndWatch.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	TSTRING s;
	s.reserve(100);
	DbgByte buffer[16];
	DbgByte *p = buffer;
	uint32_t address = 0;
	uint32_t len = MemoryRead(address, buffer, sizeof(buffer)/sizeof(DbgByte));
	for (uint32_t i = 0; i < len; i++)
	{
		if (p->safe)
		{
			char sb[10];
			snprintf(sb, sizeof(sb), "%02X", p->value);
			s += sb[0];
			s += sb[1];
			s += ' ';
		}
		else
			s += _T("** ");

		p++;
	}
	m_wndWatch.SetItemText(0, 1, s.c_str());

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void MemoryPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndWatch.SetFocus();
}
