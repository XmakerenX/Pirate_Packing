#include "GraphBin.h"
#include <iostream>

//------------------------------------------------------------------------------
GraphBin::GraphBin(const Chromozome& chromo, Configuration* conf)
{
	this->configuration = conf;
        this->chromozome.clear();
        for (int i = 0; i < chromo.size(); i++)
            this->chromozome.push_back(chromo[i]);
	this->cellsFilled = 0;
	initAndClearTheGraph();

	//continue placing items untill its not possible
	for (int i = 0; i < configuration->numberOfItems; i++)
	{
		try//try fitting a lego
		{
			int currentItemIndex = chromozome[i];
			const Item& item = configuration->items[currentItemIndex];
			Position placedItemPosition = bottomLeftFill(item);
			itemsPosition.push_back(placedItemPosition);
			cellsFilled += item.height*item.width;
		}
		catch (CantFitException cantfitEx) // break if you can't
		{
			break;
		}
	}

	freeTheGraph();
}
//------------------------------------------------------------------------------
GraphBin::GraphBin(const GraphBin& g)
{
	this->configuration = g.configuration;
	this->chromozome.clear();
	for (int i = 0; i < g.chromozome.size(); i++)
		this->chromozome.push_back(g.chromozome[i]);
	this->cellsFilled = g.cellsFilled;
	this->itemsPosition = g.itemsPosition;
	this->booleanGraphsSpaces = nullptr;
}
//------------------------------------------------------------------------------
GraphBin::GraphBin(GraphBin&& move)
	:chromozome(std::move(move.chromozome))
{
	this->configuration = move.configuration;
	this->cellsFilled = move.cellsFilled;
	this->itemsPosition = move.itemsPosition;
	this->booleanGraphsSpaces = nullptr;
}
//-------------------------------------------------------------------------------
GraphBin::~GraphBin()
{
}
//-------------------------------------------------------------------------------
GraphBin& GraphBin::operator=(const GraphBin& g)
{
	this->configuration = g.configuration;
	this->chromozome.clear();
	for (int i = 0; i < g.chromozome.size(); i++)
		this->chromozome.push_back(g.chromozome[i]);
	this->cellsFilled = g.cellsFilled;
	this->itemsPosition = g.itemsPosition;
	this->booleanGraphsSpaces = nullptr; 
	            
	return *this;
}
//-------------------------------------------------------------------------------
GraphBin& GraphBin::operator=(GraphBin&& g)
{
	this->configuration = g.configuration;
	this->chromozome = std::move(g.chromozome);
	this->cellsFilled = g.cellsFilled;
	this->itemsPosition = g.itemsPosition;
	this->booleanGraphsSpaces = nullptr; 
	    
	return *this;
}
//-------------------------------------------------------------------------------
void GraphBin::initAndClearTheGraph()
{
	booleanGraphsSpaces = new bool*[configuration->container_height];
	for (int i = 0; i < configuration->container_height; i++)
	{
		booleanGraphsSpaces[i] = new bool[configuration->container_width];//	booleansAsRow += configuration->container_width;
	}
	for (int i = 0; i < configuration->container_height; i++)
	{
		for (int j = 0; j < configuration->container_width; j++)
		{
			booleanGraphsSpaces[i][j] = false;
		}
	}
}
void GraphBin::freeTheGraph()
{
	for (int i = 0; i < configuration->container_height; i++)
	{
		delete[] this->booleanGraphsSpaces[i];
	}
	delete[] this->booleanGraphsSpaces;
}
//---------------------------------------------------------------------------------
Position GraphBin::bottomLeftFill(Item item)
{
	Position placedPosition;

	//search row by row from the leftest corner to find a match
	for (int i = 0; i < configuration->container_height - item.height;i++)
	{
		for (int j = 0; j < configuration->container_width - item.width; j++)
		{
			if (isIndexFit(i, j,item))
			{
				placedPosition.pos_y = i;
				placedPosition.pos_x = j;
				return placedPosition;
			}
		}
	}
	throw CantFitException();
}
//--------------------------------------------------------------------------------
bool GraphBin::isIndexFit(int i,int j,Item item)
{
	for (int h = i; h < i + item.height; h++)
	{
		for (int w = j; w < j + item.width; w++)
		{
			if (booleanGraphsSpaces[h][w]) return false;
		}
	}
	for (int h = i; h < i + item.height; h++)
	{
		for (int w = j; w < j + item.width; w++)
		{
			booleanGraphsSpaces[h][w] = true;
		}
	}
	return true;
}
//---------------------------------------------------------------------------------
