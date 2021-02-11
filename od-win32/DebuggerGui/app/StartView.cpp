#include "stdafx.h"
#include "DebuggerGui.h"
#include "StartView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_START_IMAGE 1

IMPLEMENT_DYNCREATE(StartView, CView)

StartView::StartView()
{
}

StartView::~StartView()
{
}

BEGIN_MESSAGE_MAP(StartView, CView)
	ON_WM_ERASEBKGND()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnDisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnDisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnDisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnDisableUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnDisableUpdate)
END_MESSAGE_MAP()

void StartView::OnInitialUpdate()
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (m_Image.GetCount() == 0)
	{
		m_Image.Load(IDR_START);
		m_Image.SetSingleImage();
		m_Image.SetTransparentColor(RGB(255, 0, 255));
	}

	pDoc->SetTitle(_T("Start Page"));
}

void StartView::OnDraw(CDC* pDC)
{
	ASSERT_VALID(pDC);

	CRect rectClient;
	GetClientRect(rectClient);

	pDC->FillSolidRect(rectClient, RGB(255, 255, 255));

	m_Image.DrawEx(pDC, rectClient, 0);
}

BOOL StartView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void StartView::OnDisableUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
