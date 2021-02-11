#include "stdafx.h"
#include "app/DebuggerGui.h"
#include "AppLookDlg.h"
#include "app/MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AppLookDlg::AppLookDlg(BOOL bStartup, CWnd* pParent /*=NULL*/) :
	CDialog(AppLookDlg::IDD, pParent), m_bStartup(bStartup)
{
	DebuggerGuiApp &app = DebuggerGuiApp::getInstance();

	m_bShowAtStartup = FALSE;
	m_bOneNoteTabs = TRUE;
	m_bDockTabColors = FALSE;
	m_bRoundedTabs = FALSE;
	m_bCustomTooltips = TRUE;
	m_nAppLook = 3;
	m_nStyle = 0;
	m_bActiveTabCloseButton = FALSE;

	m_nAppLook = app.GetInt(_T("AppLook"), 3);
	m_nStyle = app.GetInt(_T("AppStyle"), 0);;
	m_bShowAtStartup = app.GetInt(_T("ShowAppLookAtStartup"), TRUE);
	m_bOneNoteTabs = app.GetInt(_T("OneNoteTabs"), TRUE);
	m_bDockTabColors = app.GetInt(_T("DockTabColors"), FALSE);
	m_bRoundedTabs = app.GetInt(_T("RoundedTabs"), FALSE);
	m_bCustomTooltips = app.GetInt(_T("CustomTooltips"), TRUE);
	m_bActiveTabCloseButton = app.GetInt(_T("ActiveTabCloseButton"), FALSE);
}

void AppLookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STYLE, m_wndStyle);
	DDX_Control(pDX, IDC_ROUNDED_TABS, m_wndRoundedTabs);
	DDX_Control(pDX, IDC_DOCK_TAB_COLORS, m_wndDockTabColors);
	DDX_Control(pDX, IDC_ONENOTE_TABS, m_wndOneNoteTabs);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
	DDX_Check(pDX, IDC_SHOW_AT_STARTUP, m_bShowAtStartup);
	DDX_Check(pDX, IDC_ONENOTE_TABS, m_bOneNoteTabs);
	DDX_Check(pDX, IDC_DOCK_TAB_COLORS, m_bDockTabColors);
	DDX_Check(pDX, IDC_ROUNDED_TABS, m_bRoundedTabs);
	DDX_Check(pDX, IDC_CUSTOM_TOOLTIPS, m_bCustomTooltips);
	DDX_CBIndex(pDX, IDC_APP_LOOK, m_nAppLook);
	DDX_CBIndex(pDX, IDC_STYLE, m_nStyle);
	DDX_Check(pDX, IDC_ACTIVETAB_CLOSE_BUTTON, m_bActiveTabCloseButton);
}

BEGIN_MESSAGE_MAP(AppLookDlg, CDialog)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_CBN_SELENDOK(IDC_APP_LOOK, OnAppLook)
END_MESSAGE_MAP()

BOOL AppLookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
	OnAppLook();

	if (m_bStartup)
	{
		SetLook();

		if (!m_bShowAtStartup)
		{
			EndDialog(IDCANCEL);
			return TRUE;
		}

		CRect rectBtn;

		m_wndCancel.GetWindowRect(rectBtn);
		ScreenToClient(rectBtn);

		m_wndOK.MoveWindow(rectBtn);
		m_wndCancel.ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void AppLookDlg::OnOK()
{
	SetLook();
	CDialog::OnOK();
}

void AppLookDlg::applyLook()
{
	DebuggerGuiApp &app = DebuggerGuiApp::getInstance();
	
	applyLook(app.GetInt(_T("RoundedTabs"), FALSE), app.GetInt(_T("AppLook"), 3), app.GetInt(_T("AppStyle"), 0));
}

void AppLookDlg::applyLook(int appLook, bool roundedTabs, int style)
{
	switch (appLook)
	{
		case 0:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

		case 1:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

		case 2:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		break;

		case 3:
		{
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));

			if (roundedTabs)
				CTabbedPane::m_StyleTabWnd = CMFCTabCtrl::STYLE_3D_ROUNDED;

			CMFCVisualManagerVS2005::m_bRoundedAutohideButtons = roundedTabs;
		}
		break;

		case 4:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
			CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		break;

		case 5:
		{
			switch (style)
			{
			case 0:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
				break;

			case 1:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
				break;

			case 2:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
				break;

			case 3:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
				break;
			}

			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		}
		break;

		case 6:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		break;
	}
}

void AppLookDlg::SetLook()
{
	DebuggerGuiApp &app = DebuggerGuiApp::getInstance();

	CWaitCursor wait;

	UpdateData();

	MainFrame* pMainFrame = DYNAMIC_DOWNCAST(MainFrame, AfxGetMainWnd());
	if (pMainFrame != NULL)
	{
		pMainFrame->LockWindowUpdate();
	}

	CTabbedPane::m_StyleTabWnd = CMFCTabCtrl::STYLE_3D;

	applyLook(m_nAppLook, m_bRoundedTabs, m_nStyle);

	app.WriteInt(_T("AppLook"), m_nAppLook);
	app.WriteInt(_T("ShowAppLookAtStartup"), m_bShowAtStartup);
	app.WriteInt(_T("OneNoteTabs"), m_bOneNoteTabs);
	app.WriteInt(_T("DockTabColors"), m_bDockTabColors);
	app.WriteInt(_T("RoundedTabs"), m_bRoundedTabs);
	app.WriteInt(_T("CustomTooltips"), m_bCustomTooltips);
	app.WriteInt(_T("AppStyle"), m_nStyle);
	app.WriteInt(_T("ActiveTabCloseButton"), m_bActiveTabCloseButton);

	switch (m_nAppLook)
	{
		case 2: // Office 2003
		case 3: // VS.NET 2005
		case 4: // Windows XP
		case 5: // Office 2007
		case 6: // VS 2008
		{
			CWindowDC dc(NULL);
			app.m_bHiColorIcons = dc.GetDeviceCaps(BITSPIXEL) >= 16;

			CDockingManager::SetDockingMode(DT_SMART);
		}
		break;

		default:
			app.m_bHiColorIcons = FALSE;
	}

	CTabbedPane::ResetTabs();

	if (m_bCustomTooltips)
	{
		CMFCToolTipInfo params;
		params.m_bVislManagerTheme = TRUE;

		app.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &params);
	}
	else
		app.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, NULL, NULL);

	if (pMainFrame != NULL)
	{
		pMainFrame->OnChangeLook(m_bOneNoteTabs/* OneNote tabs */, m_bOneNoteTabs /* MDI tab colors*/, m_nAppLook != 0 /* VS.NET look */, m_bDockTabColors /* Dock tab colors*/, m_nAppLook == 3 || m_nAppLook == 6 /* VS 2005/2008 MDI tabs */, m_bActiveTabCloseButton);

		pMainFrame->UnlockWindowUpdate();
		pMainFrame->RedrawWindow();
	}
}

void AppLookDlg::OnApply()
{
	SetLook();

	if (!m_bStartup)
		m_wndCancel.SetWindowText(_T("Close"));
}

void AppLookDlg::OnAppLook()
{
	UpdateData();

	m_wndRoundedTabs.EnableWindow(m_nAppLook == 3);
	m_wndStyle.EnableWindow(m_nAppLook == 5);
}
