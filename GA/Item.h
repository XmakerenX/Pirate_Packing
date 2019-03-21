#pragma once
#include "../includes/structs.h"
class Item
{
public:
    Item(const Dimensions& _dim, int _value, int _id);
    ~Item();
    Dimensions dim;
    int value;
    int id;
    Color color;
};
