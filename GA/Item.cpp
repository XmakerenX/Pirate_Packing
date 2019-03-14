#include "Item.h"
#include "GA_Random.h"

/*
	Item reffers to a 3D Treasure box
*/
//------------------------------------------------------------------------------------------
Item::Item(const Configuration& configuration,int id)
{
	std::uniform_int_distribution<int> itemWidthDist(1, configuration.container_width);
	std::uniform_int_distribution<int> itemHeightDist(1, configuration.container_height);
	std::uniform_int_distribution<int> itemDepthDist(1, configuration.container_depth);
	std::uniform_int_distribution<int> itemValueDist(1, 10000);

	std::uniform_int_distribution<int> colorDist(0, 255);

	this->id = id;

	//generate a random value
	this->value = itemValueDist(Random::default_engine.getGenerator());
	//generate dimensions
	do
	{
		this->width  = itemWidthDist(Random::default_engine.getGenerator());
		this->height = itemHeightDist(Random::default_engine.getGenerator());
		this->depth  = itemDepthDist(Random::default_engine.getGenerator());
	} while (width * height*depth > (configuration.container_width * configuration.container_height*configuration.container_depth) / 6);

	//Set color to be random
	this->color.r = colorDist(Random::default_engine.getGenerator());
	this->color.g = colorDist(Random::default_engine.getGenerator());
	this->color.b = colorDist(Random::default_engine.getGenerator());

	
}
//------------------------------------------------------------------------------------
Item::Item(Dimensions dim, int item_value, int item_id)
{
	QVector3D dim2(1, 1, 1);
	this->width =  dim.w;
	this->height = dim.h;
	this->depth =  dim.d;
	this->value = item_value;
	this->id = item_id;

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
