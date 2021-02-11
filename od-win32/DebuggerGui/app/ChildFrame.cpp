#include "stdafx.h"
#include "DebuggerGui.h"
#include "StartView.h"

#include "ChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(ChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(ChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

ChildFrame::ChildFrame()
{
}

ChildFrame::~ChildFrame()
{
}

BOOL ChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_SYSMENU;

	if (!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

int ChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void ChildFrame::ActivateFrame(int nCmdShow)
{
	nCmdShow = SW_MAXIMIZE;
	CMDIChildWndEx::ActivateFrame(nCmdShow);
}

BOOL ChildFrame::IsReadOnly()
{
	return DYNAMIC_DOWNCAST(StartView, GetActiveView()) != NULL;
}

LPCTSTR ChildFrame::GetDocumentName(CObject** pObj)
{
	if (DYNAMIC_DOWNCAST(StartView, GetActiveView()) != NULL)
	{
		return g_strStartViewName;
	}
	return CMDIChildWndEx::GetDocumentName(pObj);
}
