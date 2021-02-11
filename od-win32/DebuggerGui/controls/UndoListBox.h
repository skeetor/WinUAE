#pragma once

class UndoBar;

class UndoListBox : public CListBox
{
public:
	UndoListBox(UndoBar& bar);
	virtual ~UndoListBox();

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

protected:
	UndoBar& m_Bar;
};
