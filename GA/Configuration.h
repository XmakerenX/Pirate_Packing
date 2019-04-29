#pragma once
#include <vector>
#include "../includes/structs.h"
#include "Item.h"
#include "GA_Random.h"
#include <boost/dynamic_bitset.hpp>

class Configuration
{
public:
	Configuration(const Dimensions& _dim, int _numberOfItems, Random& randomEngine = Random::default_engine);
	Configuration(const Dimensions& _dim, std::vector<Item>& givenItems);
	Configuration(const Dimensions& _dim, std::vector<Item>&& givenItems);
	Configuration(const Configuration& copy);
	Configuration(Configuration&& move);
	~Configuration();

	Configuration& operator=(const Configuration& copy);
	Configuration& operator=(Configuration&& move);
    
	void Reset(Random& randomEngine = Random::default_engine);
	void generateItems(Random& randomEngine);
	void saveToFile();
	void setBinaryUtilValues();
        

        
	Dimensions dim;
	int numberOfItems;
	std::vector<Item> items;
	int maxiumValue;
	// BinaryEncoding Utils values
	long unsigned int coordinateBits;
	long unsigned int bitsPerItem;
	long unsigned int totalBitsNum;
	boost::dynamic_bitset<> itemMask;
	boost::dynamic_bitset<> zMask;
	boost::dynamic_bitset<> yMask;
	boost::dynamic_bitset<> xMask;
	boost::dynamic_bitset<> sevenMask;
};

