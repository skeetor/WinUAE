#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "res/resource.h"

class MainFrame;

// Options what to do, when the close button is used. Using File->Exit will always exit.
typedef enum
{
	CLOSE_DO_EXIT,
	CLOSE_MINIMIZE,
	CLOSE_IGNORE
} CloseButtonOption;

class DebuggerGuiApp : public CWinAppEx
{
public:
	DebuggerGuiApp();
	virtual ~DebuggerGuiApp();

	static DebuggerGuiApp &getInstance(void);

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	bool isMenuClose(void) const { return m_menuClose; }
	CloseButtonOption getCloseOption(void) const { return m_closeOption; }
	bool InitDebugger(bool bFirstTime);
	void CloseDebugger();

	static bool isAttached(void);

public:	// Overrides
	BOOL InitInstance() override;
	int ExitInstance() override;
	BOOL SaveAllModified() override;

	// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnHelpShowStart();
	afx_msg void OnViewAppLook();
	afx_msg void OnMenuClose();

	DECLARE_MESSAGE_MAP()

protected:
	void ShowInstance();

public:
	BOOL m_bHiColorIcons;
	CMultiDocTemplate *m_pDocTemplateCpp;
	CMultiDocTemplate *m_pStartDocTemplate;
	MainFrame *m_mainFrame;

private:
	bool m_menuClose;		// true if the Menu->Exit item was selected, false if closing via button (or some other means).
	CloseButtonOption m_closeOption;
};
