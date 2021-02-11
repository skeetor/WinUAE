#pragma once

#include "ClassTreeWnd.h"

class ResourceViewBar : public CDockablePane
{
public:
	ResourceViewBar();
	virtual ~ResourceViewBar();

	void OnChangeVisualStyle();

protected:
	void FillResourceView();
	virtual BOOL CanAdjustLayout() const { return !m_bIsSliding || !m_bIsHiding; }

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
protected:
	ClassTreeWnd m_wndResourceView;
	CImageList m_ResourceViewImages;

};
