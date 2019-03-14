#include "Configuration.h"

<<<<<<< HEAD
Configuration::Configuration(int _container_width, int _container_height, int _container_depth,int _numberOfItems)
	:container_width(_container_width), 
	container_height(_container_height),
	container_depth(_container_depth),
	numberOfItems(_numberOfItems)
=======
Configuration::Configuration(Dimensions dim, int _numberOfItems)
	:container_width(dim.w), container_height(dim.h),container_depth(dim.d), numberOfItems(_numberOfItems)
>>>>>>> bdc905509d3786feb7c1ee0d52403c51e01d0c56
{
	
	items = generateItems();//This generate an empty vector of items
	for (int i = 0; i < numberOfItems; i++)//this creates items and put it inside the vector
	{
		Item item(*this,i);
		items.push_back(item);
	}
}
//------------------------------------------------------------------------------------
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
<<<<<<< HEAD
//----------------------------------------------------------------------------------------------------------
Configuration::Configuration() :container_width(0), container_height(0), container_depth(0), numberOfItems(0)
=======
//------------------------------------------------------------------------------------
Configuration::Configuration() :container_width(0), container_height(0),container_depth(0),numberOfItems(0)
>>>>>>> bdc905509d3786feb7c1ee0d52403c51e01d0c56
{
	//this constructor is needed in order to perfom
	//Configuration c1 = c2;
}
//------------------------------------------------------------------------------------
Configuration::~Configuration()
{
}
<<<<<<< HEAD
=======
//------------------------------------------------------------------------------------
>>>>>>> bdc905509d3786feb7c1ee0d52403c51e01d0c56
