#pragma once

#include <wx/treectrl.h>

template<typename T>
class TreeItemData
: public wxTreeItemData
{
public:
	TreeItemData(T data, bool attached = false)
	: m_data(data)
	, m_attached(attached)
	{
	}

	virtual ~TreeItemData(void) {}

	operator T() { return m_data;  }

	const T &getData(void) const { return m_data; }
	T &getData(void) { return m_data; }

	/**
	 * If the item data is a wxWindows pointer, it will be deleted
	 * if it is attached to some parent (i.e. Notebook page). In this
	 * case the item may not be deleted by the user, otherwise it must
	 * be deleted by the user.
	 */
	void setAttached(bool attached) { m_attached = attached; }
	bool isAttached(void) { return m_attached;  }

private:
	T m_data;
	bool m_attached : 1;
};
