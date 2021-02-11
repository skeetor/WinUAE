#pragma once

class FindComboButton : public CMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(FindComboButton)

public:
	FindComboButton() : CMFCToolBarComboBoxButton(ID_EDIT_FIND_COMBO, GetCmdMgr()->GetCmdImage(ID_EDIT_FIND), CBS_DROPDOWN)
	{
	}

	static BOOL HasFocus()
	{
		return m_bHasFocus;
	}

protected:
	virtual BOOL NotifyCommand(int iNotifyCode);

protected:
	static BOOL m_bHasFocus;
};

