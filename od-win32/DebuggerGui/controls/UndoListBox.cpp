#include "stdafx.h"
#include "UndoListBox.h"
#include "UndoBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UndoListBox::UndoListBox(UndoBar& bar) : m_Bar(bar)
{
}

UndoListBox::~UndoListBox()
{
}

BEGIN_MESSAGE_MAP(UndoListBox, CListBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void UndoListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bOutside;
	ItemFromPoint(point, bOutside);

	if (!bOutside)
	{
		m_Bar.DoUndo();
	}

	if (IsWindow(GetSafeHwnd()))
	{
		CListBox::OnLButtonUp(nFlags, point);
	}
}

void UndoListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	CListBox::OnMouseMove(nFlags, point);

	BOOL bOutside;
	int iItem = ItemFromPoint(point, bOutside);

	if (!bOutside)
	{
		SelItemRange(FALSE, iItem + 1, GetCount() - 1);

		if (iItem == 0)
		{
			SetSel(0);
		}
		else
		{
			SelItemRange(TRUE, 0, iItem);
		}

		CString strLabel;
		strLabel.Format(_T("Undo %d Actions"), iItem + 1);

		m_Bar.SetLabel(strLabel);
	}
}
