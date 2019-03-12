#include "Configuration.h"

Configuration::Configuration(int _container_width, int _container_height, int _numberOfItems)
	:container_width(_container_width), container_height(_container_height), numberOfItems(_numberOfItems)
{
	items = generateItems();
	for (int i = 0; i < numberOfItems; i++)
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
Configuration::Configuration() :container_width(0), container_height(0), numberOfItems(0)
{

	//this constructor is needed in order to perfom
	//Configuration c1 = c2;
}
//----------------------------------------------------------------------------------------------------------
Configuration::~Configuration()
{
}