#pragma once
#include "Configuration.h"
#include "../includes/structs.h"
class Configuration;
class Item
{
public:
	Item(const Configuration& configuration,int id);
	Item(int w, int h, ,int d ,int id);
	~Item();
	int width;
	int height;
    int depth;
	int id;
	Color color;

};

