#pragma once
#include "../includes/structs.h"
#include "GA_Random.h"

class Item
{
public:
    Item(const Dimensions& _dim, int _value, int _id, Random& randomEngine = Random::default_engine);
    ~Item();
    Dimensions dim;
    int value;
    int id;
    Color color;
};
