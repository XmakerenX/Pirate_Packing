#pragma once
#include "Item.h"
#include <vector>
#include "../includes/structs.h"

class Item;
class Configuration
{
	
public:
	Configuration(Dimensions dim, int _numberOfItems);
	Configuration();
	~Configuration();

	void Reset();

	static std::vector<Item> generateItems(){return std::vector<Item>();};
        
	const int container_width;
	const int container_height;
	const int container_depth;

	const int numberOfItems;
	std::vector<Item> items;
};

