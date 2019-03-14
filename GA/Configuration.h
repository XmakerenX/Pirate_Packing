#pragma once
#include "Item.h"
#include <vector>

class Item;
class Configuration
{
	
public:
	Configuration(int _container_width, int _container_height, int _container_depth,int _numberOfItems);
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

