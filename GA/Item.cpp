#include "Item.h"

/*
	Item reffers to a 3D Treasure box
*/
//------------------------------------------------------------------------------------
Item::Item(const Dimensions& _dim, int _value, int _id, Random& randomEngine/* = Random::default_engine*/)
    :dim(_dim)
{
	this->value = _value;
	this->id = _id;
	
	//Set a random color
	std::uniform_int_distribution<int> colorDist(0, 255);
	this->color.r = colorDist(randomEngine.getGenerator());
	this->color.g = colorDist(randomEngine.getGenerator());
	this->color.b = colorDist(randomEngine.getGenerator());
}
//------------------------------------------------------------------------------------
Item::~Item()
{
}
