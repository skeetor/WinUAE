#pragma once

#include "panes/WatchPane.h"
#include "panes/CPUPane.h"
#include "panes/MemoryPane.h"
#include "ClassViewBar.h"
#include "ResourceView.h"
#include "FileViewBar.h"
#include "OutputView.h"
#include "DynamicHelpView.h"
#include "PropertiesViewBar.h"

class MainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(MainFrame)

public:
	MainFrame();
	virtual ~MainFrame();

	static CMFCToolBarComboBoxButton* GetFindCombo();

	void OnChangeLook(BOOL bOneNoteTabs, BOOL bMDITabColors, BOOL bIsVSDotNetLook, BOOL bDockTabColors, BOOL bMDITabsVS2005Look, BOOL bActiveTabCloseButton);

public: // Overrides
	BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL) override;
	BOOL DestroyWindow() override;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual CMDIChildWndEx* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* pObj);
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnHelpKeyboardmap();
	afx_msg void OnFileNewProject();
	afx_msg void OnFileNewBlankSolution();
	afx_msg void OnFileOpenSolution();
	afx_msg void OnFileCloseSolution();
	afx_msg void OnFileSaveAll();
	afx_msg void OnViewRefresh();
	afx_msg void OnToolsConnectToDatabase();
	afx_msg void OnToolsDebugProcesses();
	afx_msg void OnViewFullScreen();
	afx_msg void OnToolsOptions();
	afx_msg void OnHelpWeb();
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnClose();
	afx_msg void OnViewClass();
	afx_msg void OnUpdateViewClass(CCmdUI* pCmdUI);
	afx_msg void OnViewFile();
	afx_msg void OnUpdateViewFile(CCmdUI* pCmdUI);
	afx_msg void OnViewResource();
	afx_msg void OnUpdateViewResource(CCmdUI* pCmdUI);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnViewDynamichelp();
	afx_msg void OnUpdateViewDynamichelp(CCmdUI* pCmdUI);
	afx_msg void OnMdiTabbedDocument();
	afx_msg void OnUpdateMdiTabbedDocument(CCmdUI* pCmdUI);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg LRESULT OnGetTabToolTip(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnHelpCustomizeToolbars(WPARAM wp, LPARAM lp);
	LRESULT OnToolbarCreateNew(WPARAM wp,LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	afx_msg void OnViewWatch();
	afx_msg void OnUpdateViewWatch(CCmdUI* pCmdUI);

	afx_msg void OnViewCPU();
	afx_msg void OnUpdateViewCPU(CCmdUI* pCmdUI);

	afx_msg void OnViewMemory();
	afx_msg void OnUpdateViewMemory(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingBars();
	void SetDockingBarsIcons(BOOL bHiColorIcons);

private:
	BOOL IsEditViewActive();

private:
	CMFCMenuBar m_wndMenuBar;
	CMFCStatusBar m_wndStatusBar;
	CMFCToolBar m_wndToolBar;
	CMFCToolBar m_wndToolbarBuild;
	CMFCToolBar m_wndToolbarEdit;
	CMFCDropDownToolBar m_wndToolbarResource;

	ClassViewBar m_wndClassView;
	ResourceViewBar m_wndResourceView;
	FileViewBar m_wndFileView;

	DynamicHelpViewBar m_wndDynamicHelpView;
	OutputViewBar m_wndOutputView;
	PropertiesViewBar m_wndPropertiesBar;

	CMFCToolBarImages m_UserImages;

	WatchPane m_watchPane;
	CPUPane m_cpuPane;
	MemoryPane m_memoryPane;
};
