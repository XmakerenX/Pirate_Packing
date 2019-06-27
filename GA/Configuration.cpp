#include "Configuration.h"
#include "GA_Random.h"
#include <fstream>
#include <qdir.h>

//-----------------------------------------------------------------------------------------------
// Name : Configuration(constructor)
// Input: dim - the container width, height and width
//        numberOfItems - how many item to generate in the Configuration
//        randomEngine   - the random number generator to use for random numbers
// Output: A new Configuration with a container with size dim and numberOfItems random items
// Action: Creates a Configuration with the given dim and numberOfItems random items
//-----------------------------------------------------------------------------------------------
Configuration::Configuration(const Dimensions& _dim, int _numberOfItems, Random& randomEngine/* = Random::default_engine*/)
	:dim(_dim), numberOfItems(_numberOfItems)
{
	generateItems(randomEngine);
	setBinaryUtilValues();
}

//-----------------------------------------------------------------------------------------------
// Name : Configuration(constructor)
// Input: dim - the container width, height and width
//        givenItems - the items to include in the Configuration
// Output: A new Configuration with a container with size dim and the given items
// Action: Creates a Configuration with the given dim and a copy of given items
//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
// Name : Configuration(constructor)
// Input: dim - the container width, height and width
//        givenItems - the items to include in the Configuration
// Output: A new Configuration with a container with size dim and the given items
// Action: Creates a Configuration with the given dim and the given items
//-----------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
// Name:   Configuration(copy constructor)
// Input:  Configuration to be copied
// Output: A copy Configuration of the given Configuration
// Action: copy constructor for Configuration
//--------------------------------------------------------------------------------------------------
Configuration::Configuration(const Configuration& copy)
{
	dim = copy.dim;
	items = copy.items;
	numberOfItems = items.size();
	maxiumValue = copy.maxiumValue;
    
	setBinaryUtilValues();
}

//--------------------------------------------------------------------------------------------------
// Name:   Configuration(move constructor)
// Input:  Configuration to be moved
// Output: A moved Configuration 
// Action: move constructor for Configuration
//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
// Name:   operator= (copy)
// Input:  Configuration to be copied
// Output: A copy of the given Configuration
// Action: copy assignment operator for Configuration
//--------------------------------------------------------------------------------------------------
Configuration& Configuration::operator=(const Configuration& copy)
{
	dim = copy.dim;
	items = copy.items;
	numberOfItems = items.size();
	maxiumValue = copy.maxiumValue;
    
	setBinaryUtilValues();
    
	return *this;
}

//--------------------------------------------------------------------------------------------------
// Name:   operator= (move)
// Input:  Configuration to be moved
// Output: A moved Configuration
// Action: move assignment operator for Configuration
//--------------------------------------------------------------------------------------------------
Configuration& Configuration::operator=(Configuration&& move)
{
	dim = move.dim;
	items = std::move(move.items);
	numberOfItems = items.size();
	maxiumValue = move.maxiumValue;
    
	setBinaryUtilValues();
    
	return *this;
}

//--------------------------------------------------------------------------------------------------
// Name: Reset
//--------------------------------------------------------------------------------------------------
void Configuration::Reset(Random& randomEngine/* = Random::default_engine*/)
{
	generateItems(randomEngine);
}

//--------------------------------------------------------------------------------------------------
// Name:   generateItems
// Input:  randomEngine - the random number generator to use for random numbers
// Output: random generated items for the Configuration
// Action: generate random items for the Configuration
//--------------------------------------------------------------------------------------------------
void Configuration::generateItems(Random& randomEngine)
{
	std::uniform_int_distribution<int> itemWidthDist(1, dim.w);
	std::uniform_int_distribution<int> itemHeightDist(1, dim.h);
	std::uniform_int_distribution<int> itemDepthDist(1, dim.d);
	std::uniform_int_distribution<int> itemValueDist(1, 10000);
	std::uniform_int_distribution<int> sizeDist(1, 20);
	
	int sizeFactor = sizeDist(randomEngine.getGenerator());
	std::cout << "size Factor " << sizeFactor << "\n";
    
	maxiumValue = 0;
	items.clear();    
	for (int i = 0; i < numberOfItems; i++)
	{
		//generate a random value
		int itemValue = itemValueDist(randomEngine.getGenerator());
		//generate dimensions
		Dimensions itemDim;
		do
		{
			itemDim.w  = itemWidthDist(randomEngine.getGenerator());
			itemDim.h = itemHeightDist(randomEngine.getGenerator());
			itemDim.d  = itemDepthDist(randomEngine.getGenerator());
		} while (itemDim.w * itemDim.h * itemDim.d > (dim.w * dim.h * dim.d) / sizeFactor);

		maxiumValue += itemValue;
		items.emplace_back(itemDim, itemValue, i, randomEngine);
	}
}

//--------------------------------------------------------------------------------------------------
// Name:   saveToFile
// Input:  this Configuration data
// Output: a file in the Config folder containing the Configuration data
// Action: Save the Configuration to file
//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
// Name:   setBinaryUtilValues
// Input:  this Configuration data
// Output: help variables for binaryEncoding are set
// Action: sets the binaryEncoding helper variables
//--------------------------------------------------------------------------------------------------
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
