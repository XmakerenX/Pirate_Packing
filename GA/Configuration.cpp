#include "Configuration.h"

Configuration::Configuration(Dimensions dim, int _numberOfItems)
	:container_width(dim.w), container_height(dim.h),container_depth(dim.d), numberOfItems(_numberOfItems)
{
	
	items = generateItems();//This generate an empty vector of items
	for (int i = 0; i < numberOfItems; i++)//this creates items and put it inside the vector
	{
		Item item(*this,i);
		items.push_back(item);
	}
}
//----------------------------------------------------------------------------------------------------------
void Configuration::Reset()
{
	items.clear();
	items = generateItems();
	for (int i = 0; i < numberOfItems; i++)
	{
		Item item(*this, i);
		items.push_back(item);
	}
}
//----------------------------------------------------------------------------------------------------------
Configuration::Configuration() :container_width(0), container_height(0),container_depth(0),numberOfItems(0)
{
	//this constructor is needed in order to perfom
	//Configuration c1 = c2;
}
//----------------------------------------------------------------------------------------------------------
Configuration::~Configuration()
{
}