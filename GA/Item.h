#pragma once
#include "Configuration.h"
#include "../includes/structs.h"
class Configuration;
class Item
{
public:
	Item(const Configuration& configuration,int id);
<<<<<<< HEAD
	Item(int w, int h, ,int d ,int id);
	~Item();
	int width;
	int height;
    int depth;
=======
	Item::Item(Dimensions dim, int item_value, int item_id);
	~Item();
	int width;
	int height;
	int depth;
	int value;
>>>>>>> bdc905509d3786feb7c1ee0d52403c51e01d0c56
	int id;
	Color color;

};

