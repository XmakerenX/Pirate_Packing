#pragma once
#include "Item.h"
#include <vector>
#include "../includes/structs.h"

class Configuration
{
public:
	Configuration(const Dimensions& _dim, int _numberOfItems);
	Configuration(const Dimensions& _dim, std::vector<Item>& givenItems);
	~Configuration();

	void Reset();
	void generateItems();
        
	const Dimensions dim;
	const int numberOfItems;
	std::vector<Item> items;
	int maxiumValue;
};

