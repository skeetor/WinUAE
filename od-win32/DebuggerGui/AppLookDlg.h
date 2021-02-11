#pragma once

class AppLookDlg : public CDialog
{
public:
	AppLookDlg(BOOL bStartup, CWnd* pParent = NULL);

	enum { IDD = IDD_APP_LOOK };

	static void applyLook(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnAppLook();

	DECLARE_MESSAGE_MAP()

	const BOOL m_bStartup;

	void SetLook();

	static void applyLook(int appLook, bool roundedTabs, int style);

public:
	CComboBox m_wndStyle;
	CButton m_wndRoundedTabs;
	CButton m_wndDockTabColors;
	CButton m_wndOneNoteTabs;
	CButton m_wndOK;
	CButton m_wndCancel;
	BOOL m_bShowAtStartup;
	BOOL m_bOneNoteTabs;
	BOOL m_bDockTabColors;
	BOOL m_bRoundedTabs;
	BOOL m_bCustomTooltips;
	int m_nAppLook;
	int m_nStyle;
	BOOL m_bActiveTabCloseButton;
};
