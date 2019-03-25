#include "Configuration.h"
#include "GA_Random.h"

Configuration::Configuration(const Dimensions& _dim, int _numberOfItems)
	:dim(_dim), numberOfItems(_numberOfItems)
{
	generateItems();
}
//------------------------------------------------------------------------------------
Configuration::Configuration(const Dimensions& _dim, std::vector<Item>& givenItems)
	: dim(_dim), numberOfItems(givenItems.size())
{
	for (Item item : givenItems)
	{
		this->items.emplace_back(item);
	}
}
//------------------------------------------------------------------------------------
void Configuration::Reset()
{
	generateItems();
}
//------------------------------------------------------------------------------------
void Configuration::generateItems()
{
	std::uniform_int_distribution<int> itemWidthDist(1, dim.w);
	std::uniform_int_distribution<int> itemHeightDist(1, dim.h);
	std::uniform_int_distribution<int> itemDepthDist(1, dim.d);
	std::uniform_int_distribution<int> itemValueDist(1, 10000);
	
	maxiumValue = 0;
	items.clear();    
	for (int i = 0; i < numberOfItems; i++)
	{
		//generate a random value
		int itemValue = itemValueDist(Random::default_engine.getGenerator());
		//generate dimensions
		Dimensions itemDim;
		do
		{
			itemDim.w  = itemWidthDist(Random::default_engine.getGenerator());
			itemDim.h = itemHeightDist(Random::default_engine.getGenerator());
			itemDim.d  = itemDepthDist(Random::default_engine.getGenerator());
		} while (itemDim.w * itemDim.h * itemDim.d > (dim.w * dim.h * dim.d) / 6);

		maxiumValue += itemValue;
		items.emplace_back(itemDim, itemValue, i);
    }
}
//------------------------------------------------------------------------------------
Configuration::~Configuration()
{

}
//------------------------------------------------------------------------------------
