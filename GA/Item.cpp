#include "Item.h"
#include "GA_Random.h"

/*
	Item reffers to a 3D Treasure box
*/
//------------------------------------------------------------------------------------
Item::Item(const Dimensions& _dim, int _value, int _id)
    :dim(_dim)
{
	this->value = _value;
	this->id = _id;
	
	//Set a random color
	std::uniform_int_distribution<int> colorDist(0, 255);
	this->color.r = colorDist(Random::default_engine.getGenerator());
	this->color.g = colorDist(Random::default_engine.getGenerator());
	this->color.b = colorDist(Random::default_engine.getGenerator());
}
//------------------------------------------------------------------------------------
Item::~Item()
{
}
