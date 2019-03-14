#pragma once
#include "Item.h"
#include <vector>
#include "../includes/structs.h"

class Item;
class Configuration
{
	
public:
<<<<<<< HEAD
	Configuration(int _container_width, int _container_height, int _container_depth,int _numberOfItems);
=======
	Configuration(Dimensions dim, int _numberOfItems);
>>>>>>> bdc905509d3786feb7c1ee0d52403c51e01d0c56
	Configuration();
	~Configuration();

	void Reset();

	static std::vector<Item> generateItems(){return std::vector<Item>();};
        
	const int container_width;
	const int container_height;
<<<<<<< HEAD
    const int container_depth;
=======
	const int container_depth;

>>>>>>> bdc905509d3786feb7c1ee0d52403c51e01d0c56
	const int numberOfItems;
	std::vector<Item> items;
};

