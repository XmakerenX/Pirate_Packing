#include "Configuration.h"
#include "GA_Random.h"
#include <fstream>
#include <qdir.h>

Configuration::Configuration(const Dimensions& _dim, int _numberOfItems)
	:dim(_dim), numberOfItems(_numberOfItems)
{
	generateItems();
	setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::Configuration(const Dimensions& _dim, std::vector<Item>& givenItems)
	: dim(_dim), numberOfItems(givenItems.size()), items(givenItems)
{
	maxiumValue = 0;        
	for(Item& item : items)
	{
		maxiumValue += item.value;
	}
	setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::Configuration(const Dimensions& _dim, std::vector<Item>&& givenItems)
	: dim(_dim), numberOfItems(givenItems.size()), items(std::move(givenItems))
{
	maxiumValue = 0;        
	for(Item& item : items)
	{
		maxiumValue += item.value;
	}
	setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::Configuration(const Configuration& copy)
{
	dim = copy.dim;
	items = copy.items;
	numberOfItems = items.size();
	maxiumValue = copy.maxiumValue;
    
	setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::Configuration(Configuration&& move)
{
	dim = move.dim;
	items = std::move(move.items);
	numberOfItems = items.size();
	maxiumValue = move.maxiumValue;
    
	setBinaryUtilValues();
}
//------------------------------------------------------------------------------------
Configuration::~Configuration()
{

}
//------------------------------------------------------------------------------------
Configuration& Configuration::operator=(const Configuration& copy)
{
	dim = copy.dim;
	items = copy.items;
	numberOfItems = items.size();
	maxiumValue = copy.maxiumValue;
    
	setBinaryUtilValues();
    
	return *this;
}
//------------------------------------------------------------------------------------
Configuration& Configuration::operator=(Configuration&& move)
{
	dim = move.dim;
	items = std::move(move.items);
	numberOfItems = items.size();
	maxiumValue = move.maxiumValue;
    
	setBinaryUtilValues();
    
	return *this;
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
void Configuration::saveToFile()
{
	if (!QDir("Config").exists())
		if (!QDir().mkdir("Config"))
			return;

	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        

	std::string timeAndDate(30, '\0');
	std::strftime(&timeAndDate[0], timeAndDate.size(), "%Y-%m-%d_%H-%M-%S.txt", std::localtime(&now));
	std::string fileName = "Config/" + timeAndDate;
    
	std::ofstream file(fileName);
	file << dim.w << " " << dim.h << " " << dim.d << "\n";
    
	for (const Item& item : items)
		file << item.dim.d << " " << item.dim.h << " " << item.dim.d << " " << item.value << "\n"; 
        
        file.close();
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
