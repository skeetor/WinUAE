#include "stdafx.h"
#include "DebuggerGui.h"

#include "MainFrame.h"
#include "ChildFrame.h"
#include "DebuggerGuiDoc.h"
#include "DebuggerGuiView.h"
#include "StartView.h"
#include "AppLookDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool gAttached = false;

BEGIN_MESSAGE_MAP(DebuggerGuiApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnMenuClose)
	ON_COMMAND(ID_HELP_SHOW_START, OnHelpShowStart)
	ON_COMMAND(ID_VIEW_APP_LOOK, OnViewAppLook)
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

static DebuggerGuiApp gDebuggerGui;

DebuggerGuiApp::DebuggerGuiApp()
{
	m_pStartDocTemplate = NULL;
	m_pDocTemplateCpp = NULL;

	m_bHiColorIcons = FALSE;
	m_mainFrame = nullptr;
	m_menuClose = false;
	m_closeOption = CloseButtonOption::CLOSE_DO_EXIT;
}

DebuggerGuiApp::~DebuggerGuiApp()
{
}

DebuggerGuiApp &DebuggerGuiApp::getInstance(void)
{
	return gDebuggerGui;
}

bool DebuggerGuiApp::isAttached(void)
{
	return gAttached;
}

BOOL DebuggerGuiApp::InitInstance()
{
	LoadStdProfileSettings();

	AfxEnableControlContainer();

	SetRegistryKey(_T("WinUAE"));
	SetRegistryBase(_T("Settings"));

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	EnableUserTools(ID_TOOLS_ENTRY, ID_USER_TOOL1, ID_USER_TOOL10, RUNTIME_CLASS(CUserTool), IDR_MENU_ARGS, IDR_MENU_DIRS);

	m_pDocTemplateCpp = new CMultiDocTemplate(IDR_DEVTYPE_CPP, RUNTIME_CLASS(DebuggerGuiDoc), RUNTIME_CLASS(ChildFrame), RUNTIME_CLASS(DebuggerGuiView));
	AddDocTemplate(m_pDocTemplateCpp);

	MainFrame* pMainFrame = new MainFrame;

	m_closeOption = (CloseButtonOption)GetInt(_T("CloseButton"), CLOSE_IGNORE);

	EnableLoadWindowPlacement(FALSE);

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_pMainWnd = pMainFrame;
	m_mainFrame = pMainFrame;

#ifndef _USRDLL
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		if (!pMainFrame->LoadMDIState(GetRegSectionPath()))
		{
			m_pStartDocTemplate->OpenDocumentFile(NULL);
		}
	}
	else
	{
		if (!ProcessShellCommand(cmdInfo))
			return false;
	}
#endif

	ShowInstance();

	m_pMainWnd->DragAcceptFiles();

	return true;
}

bool DebuggerGuiApp::InitDebugger(bool firstTime)
{
	if (firstTime)
	{
		if (!AfxOleInit())
		{
			AfxMessageBox(IDP_OLE_INIT_FAILED);
			return false;
		}
	}

	gAttached = true;

	return true;
}

void DebuggerGuiApp::CloseDebugger()
{
	m_menuClose = true;
	m_mainFrame->SendMessage(WM_COMMAND, ID_APP_EXIT, 0);
}

void DebuggerGuiApp::ShowInstance()
{
	if (!ReloadWindowPlacement(m_mainFrame))
	{
		m_mainFrame->ShowWindow(m_nCmdShow);
		m_mainFrame->UpdateWindow();
	}

	AppLookDlg::applyLook();
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void DebuggerGuiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void DebuggerGuiApp::OnMenuClose()
{
	m_menuClose = true;
	CWinApp::OnAppExit();
}

void DebuggerGuiApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu(_T("Edit Context menu"), IDR_CONTEXT_MENU);
	GetContextMenuManager()->AddMenu(_T("Resource"), IDR_POPUP_RESOURCE);
	GetContextMenuManager()->AddMenu(_T("Solution Explorer"), IDR_POPUP_SOLUTION);
}

void DebuggerGuiApp::LoadCustomState()
{
}

void DebuggerGuiApp::SaveCustomState()
{
}

int DebuggerGuiApp::ExitInstance()
{
	return CWinAppEx::ExitInstance();
}

void DebuggerGuiApp::OnHelpShowStart()
{
	ASSERT_VALID(m_pStartDocTemplate);

	POSITION pos = m_pStartDocTemplate->GetFirstDocPosition();
	if (pos == NULL)
	{
		m_pStartDocTemplate->OpenDocumentFile(NULL);
		return;
	}

	CDocument* pDoc = m_pStartDocTemplate->GetNextDoc(pos);
	ASSERT_VALID(pDoc);

	pos = pDoc->GetFirstViewPosition();
	ASSERT(pos != NULL);

	CView* pView = pDoc->GetNextView(pos);
	ASSERT_VALID(pView);

	CFrameWnd* pFrame = pView->GetParentFrame();
	ASSERT_VALID(pFrame);

	MainFrame* pMainFrame = (MainFrame*) AfxGetMainWnd();
	ASSERT_VALID(pMainFrame);

	::SendMessage(pMainFrame->m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM) pFrame->GetSafeHwnd(), 0);
}

void DebuggerGuiApp::OnViewAppLook()
{
	AppLookDlg dlg(FALSE, m_pMainWnd);
	dlg.DoModal();
}

BOOL DebuggerGuiApp::SaveAllModified()
{
	if (!CWinAppEx::SaveAllModified())
	{
		return FALSE;
	}

	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, m_pMainWnd);
	if (pMainFrame != NULL)
	{
		pMainFrame->SaveMDIState(GetRegSectionPath());
	}

	return TRUE;
}

