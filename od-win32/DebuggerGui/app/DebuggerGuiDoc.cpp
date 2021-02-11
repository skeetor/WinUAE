#include "stdafx.h"
#include "DebuggerGui.h"

#include "DebuggerGuiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(DebuggerGuiDoc, CDocument)

BEGIN_MESSAGE_MAP(DebuggerGuiDoc, CDocument)
	//{{AFX_MSG_MAP(DebuggerGuiDoc)
	ON_COMMAND(ID_DUMMY_SELECT_ACTIVE_CONFIGURATION, OnDummySelectActiveConfiguration)
	ON_COMMAND(ID_DUMMY_BUILD, &DebuggerGuiDoc::OnDummyBuild)
	ON_COMMAND(ID_DUMMY_COMPILE, &DebuggerGuiDoc::OnDummyCompile)
	ON_COMMAND(ID_DUMMY_EXECUTE, &DebuggerGuiDoc::OnDummyExecute)
	ON_COMMAND(ID_DUMMY_GO, &DebuggerGuiDoc::OnDummyGo)
	ON_COMMAND(ID_DUMMY_REBUILD_ALL, &DebuggerGuiDoc::OnDummyRebuildAll)
	ON_COMMAND(ID_DUMMY_REMOVE_ALL_BREAKPOINTS, &DebuggerGuiDoc::OnDummyRemoveAllBreakpoints)
	ON_COMMAND(ID_DUMMY_CLEAN, &DebuggerGuiDoc::OnDummyClean)
	ON_COMMAND(ID_NEW_DIALOG, OnNewDialog)
	ON_COMMAND(ID_NEW_ACCELERATOR, OnNewAccelerator)
	ON_COMMAND(ID_NEW_BITMAP, OnNewBitmap)
	ON_COMMAND(ID_NEW_CURSOR, OnNewCursor)
	ON_COMMAND(ID_NEW_ICON, OnNewIcon)
	ON_COMMAND(ID_NEW_MENU, OnNewMenu)
	ON_COMMAND(ID_NEW_STRING_TABLE, OnNewStringTable)
	ON_COMMAND(ID_NEW_TOOLBAR, OnNewToolbar)
	ON_COMMAND(ID_NEW_VERSION, OnNewVersion)
	ON_CBN_SELENDOK(ID_DUMMY_SELECT_ACTIVE_CONFIGURATION, OnDummySelectActiveConfiguration)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DebuggerGuiDoc::DebuggerGuiDoc()
{
}

DebuggerGuiDoc::~DebuggerGuiDoc()
{
}

BOOL DebuggerGuiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

void DebuggerGuiDoc::OnDummySelectActiveConfiguration()
{
	CMFCToolBarComboBoxButton* pSrcCombo = NULL;

	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons(ID_DUMMY_SELECT_ACTIVE_CONFIGURATION, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition();
			pSrcCombo == NULL && posCombo != NULL;)
		{
			CMFCToolBarComboBoxButton* pCombo = DYNAMIC_DOWNCAST(CMFCToolBarComboBoxButton, listButtons.GetNext(posCombo));

			if (pCombo != NULL && CMFCToolBar::IsLastCommandFromButton(pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}

	if (pSrcCombo != NULL)
	{
		ASSERT_VALID(pSrcCombo);

		LPCTSTR lpszSelItem = pSrcCombo->GetItem();
		CString strSelItem = (lpszSelItem == NULL) ? _T("") : lpszSelItem;

		AfxMessageBox(strSelItem);
	}
	else
	{
		AfxMessageBox(_T("Show \"Set Active Configuration\" dialog...."));
	}
}

void DebuggerGuiDoc::Serialize(CArchive& ar)
{
	reinterpret_cast<CEditView*>(m_viewList.GetHead())->SerializeRaw(ar);
}

void DebuggerGuiDoc::OnDummyBuild()
{
}

void DebuggerGuiDoc::OnDummyCompile()
{
}

void DebuggerGuiDoc::OnDummyExecute()
{
}

void DebuggerGuiDoc::OnDummyGo()
{
}

void DebuggerGuiDoc::OnDummyRebuildAll()
{
}

void DebuggerGuiDoc::OnDummyRemoveAllBreakpoints()
{
}

void DebuggerGuiDoc::OnDummyClean()
{
}

void DebuggerGuiDoc::OnNewDialog()
{
	AfxMessageBox(_T("New Dialog"));
}

void DebuggerGuiDoc::OnNewAccelerator()
{
	AfxMessageBox(_T("New Accelerator"));
}

void DebuggerGuiDoc::OnNewBitmap()
{
	AfxMessageBox(_T("New Bitmap"));
}

void DebuggerGuiDoc::OnNewCursor()
{
	AfxMessageBox(_T("New Cursor"));
}

void DebuggerGuiDoc::OnNewIcon()
{
	AfxMessageBox(_T("New Icon"));
}

void DebuggerGuiDoc::OnNewMenu()
{
	AfxMessageBox(_T("New Menu"));
}

void DebuggerGuiDoc::OnNewStringTable()
{
	AfxMessageBox(_T("New StringTable"));
}

void DebuggerGuiDoc::OnNewToolbar()
{
	AfxMessageBox(_T("New Toolbar"));
}

void DebuggerGuiDoc::OnNewVersion()
{
	AfxMessageBox(_T("New Version"));
}
