#include "Item.h"
#include "GA_Random.h"

/*
	Item reffers to a 2D lego rectengle
*/
//------------------------------------------------------------------------------------------
Item::Item(const Configuration& configuration,int id)
{
	std::uniform_int_distribution<int> itemWidthDist(1, configuration.container_width);
	std::uniform_int_distribution<int> itemHeightDist(1, configuration.container_height);
    std::uniform_int_distribution<int> itemDepthtDist(1, configuration.container_depth);
	std::uniform_int_distribution<int> colorDist(0, 255);

    unsigned int containerVolume = configuration.container_width * configuration.container_height * configuration.container_depth;
	this->id = id;
	do
	{
		this->width = itemWidthDist(Random::default_engine.getGenerator());
		this->height = itemHeightDist(Random::default_engine.getGenerator());
        this->depth = itemDepthtDist(Random::default_engine.getGenerator());
	} while (width * height * depth > (containerVolume) / 6);

	//Set color to be random
	this->color.r = colorDist(Random::default_engine.getGenerator());
	this->color.g = colorDist(Random::default_engine.getGenerator());
	this->color.b = colorDist(Random::default_engine.getGenerator());

	
}

//--------------------------------------
Item::Item(int w,int h,int d,int id)
{
	this->width = w;
	this->height= h;
    this->depth = d;
	this->id = id;
}
//--------------------------------------
Item::~Item()
{
}
