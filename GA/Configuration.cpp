#include "Configuration.h"
#include "GA_Random.h"

Configuration::Configuration(const Dimensions& _dim, int _numberOfItems)
	:dim(_dim), numberOfItems(_numberOfItems)
{
	generateItems();
    setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::Configuration(const Dimensions& _dim, std::vector<Item>& givenItems)
	: dim(_dim), numberOfItems(givenItems.size())
{
	for (Item& item : givenItems)
	{
		this->items.emplace_back(item.dim,item.value,item.id);
	}
	setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::~Configuration()
{

}
//-----------------------------------------------------------------------------------
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
    std::uniform_int_distribution<int> sizeDist(1, 20);
	
    int sizeFactor = sizeDist(Random::default_engine.getGenerator());
    std::cout << "size Factor " << sizeFactor << "\n";
    
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
        } while (itemDim.w * itemDim.h * itemDim.d > (dim.w * dim.h * dim.d) / sizeFactor);

		maxiumValue += itemValue;
		items.emplace_back(itemDim, itemValue, i);
    }
}
//------------------------------------------------------------------------------------
void Configuration::setBinaryUtilValues()
{
    long unsigned int maxDimensionValue = std::max(dim.w, std::max(dim.h, dim.d));
    coordinateBits = std::ceil(std::log2(maxDimensionValue));
    bitsPerItem = 4 + 3 * coordinateBits;
    
    totalBitsNum = items.size()*4 + 3 * items.size() * coordinateBits;
    
    long unsigned int itemMaskValue = std::pow(2, bitsPerItem);
    itemMaskValue--;
    itemMask = boost::dynamic_bitset<>(totalBitsNum, itemMaskValue);
    
    long unsigned int zMaskValue = std::pow(2, coordinateBits);
    zMaskValue--;
    zMask = boost::dynamic_bitset<>(totalBitsNum, zMaskValue);
    yMask = zMask << coordinateBits;
    xMask = yMask << coordinateBits;
    
    sevenMask = boost::dynamic_bitset<>(totalBitsNum, 7);
}
//------------------------------------------------------------------------------------
