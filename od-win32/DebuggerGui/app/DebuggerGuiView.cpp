#include "stdafx.h"
#include "MainFrame.h"
#include "DebuggerGui.h"

#include "DebuggerGuiDoc.h"
#include "DebuggerGuiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(DebuggerGuiView, CEditView)

BEGIN_MESSAGE_MAP(DebuggerGuiView, CEditView)
	ON_COMMAND(ID_FILE_PRINT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &DebuggerGuiView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_TOGGLEBREAKPOINT, &DebuggerGuiView::OnEditTogglebreakpoint)
	ON_COMMAND(ID_EDIT_FIND_COMBO, OnFind)
END_MESSAGE_MAP()

DebuggerGuiView::DebuggerGuiView()
{
}

DebuggerGuiView::~DebuggerGuiView()
{
}

BOOL DebuggerGuiView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL); // Enable word-wrapping

	return bPreCreated;
}

BOOL DebuggerGuiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return CEditView::OnPreparePrinting(pInfo);
}

void DebuggerGuiView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void DebuggerGuiView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CEditView::OnEndPrinting(pDC, pInfo);
}

void DebuggerGuiView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

void DebuggerGuiView::OnEditTogglebreakpoint()
{
}

void DebuggerGuiView::OnFind()
{
	CString strFindText;
	VerifyFindString(((MainFrame*)AfxGetMainWnd())->GetFindCombo(), strFindText);

	if (!strFindText.IsEmpty())
	{
		FindText(strFindText);
	}
}

void DebuggerGuiView::VerifyFindString(CMFCToolBarComboBoxButton* pFindCombo, CString& strFindText)
{
	if (pFindCombo == NULL)
	{
		return;
	}

	BOOL bIsLastCommandFromButton = CMFCToolBar::IsLastCommandFromButton(pFindCombo);

	if (bIsLastCommandFromButton)
	{
		strFindText = pFindCombo->GetText();
	}

	CComboBox* pCombo = pFindCombo->GetComboBox();

	if (!strFindText.IsEmpty())
	{
		const int nCount = pCombo->GetCount();
		int ind = 0;
		CString strCmpText;

		while (ind < nCount)
		{
			pCombo->GetLBText(ind, strCmpText);

			if (strCmpText.GetLength() == strFindText.GetLength())
			{
				if (strCmpText == strFindText)
					break;
			}

			ind++;
		}

		if (ind < nCount)
		{
			pCombo->DeleteString(ind);
		}

		pCombo->InsertString(0,strFindText);
		pCombo->SetCurSel(0);

		if (!bIsLastCommandFromButton)
		{
			pFindCombo->SetText(strFindText);
		}
	}
}
