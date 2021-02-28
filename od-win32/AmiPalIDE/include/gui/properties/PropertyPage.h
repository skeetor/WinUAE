#pragma once

#include "controls/TreeItemData.h"
#include "gui/properties/PropertyPagePanel.h"

class PropertyPagePanel;

class PropertyPage
: public TreeItemData<PropertyPagePanel *>
{
public:
	PropertyPage(PropertyPagePanel *panel)
	: TreeItemData(panel, false)
	{
	}

	~PropertyPage(void) override {}
};
