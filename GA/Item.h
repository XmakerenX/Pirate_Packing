#pragma once
#include "Configuration.h"
#include "../includes/structs.h"
class Configuration;
class Item
{
public:
    Item(const Configuration& configuration,int id);
    Item(Dimensions dim, int item_value, int item_id);
    ~Item();
    int width;
    int height;
    int depth;
    int value;
    int id;
    Color color;
};
