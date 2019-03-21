#pragma once
#include "Item.h"
#include <vector>
#include "../includes/structs.h"

class Item;
class Configuration
{
	
public:
	Configuration(Dimensions _dim, int _numberOfItems);
	Configuration();
	~Configuration();

	void Reset();

	static std::vector<Item> generateItems(){return std::vector<Item>();};
        
    const Dimensions dim;
	const int numberOfItems;
    
    int maxiumValue;
	std::vector<Item> items;
};

