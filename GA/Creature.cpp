#include "Creature.h"
#include "GraphBin.h"
#include <iostream>
#include <algorithm>

//------------------------------------------------------------
Creature::Creature(Chromozome newChromozome,Configuration* config)
	:creatureGraphBin(newChromozome,config)
{
	this->chromozome = newChromozome;
	this->configuration = config;
	this->fitness = calculateFittness();

}
//------------------------------------------------------------
Creature::Creature(const Creature& c)
	:creatureGraphBin(c.creatureGraphBin)
{
	this->chromozome = c.chromozome;
	this->configuration = c.configuration;
	this->fitness = c.fitness;
}
//------------------------------------------------------------
Creature::Creature(Creature&& move)
	:chromozome(std::move(move.chromozome)), creatureGraphBin(std::move(move.creatureGraphBin))
{
	this->configuration = move.configuration;
	this->fitness = move.fitness;
}
//------------------------------------------------------------
Creature::~Creature()
{}
//------------------------------------------------------------
Creature& Creature::operator=(const Creature& c)
{
	this->chromozome = c.chromozome;
	this->creatureGraphBin = c.creatureGraphBin;
	this->configuration = c.configuration;
	this->fitness = c.fitness;
	            
	return *this;
}
//------------------------------------------------------------
Creature& Creature::operator=(Creature&& c)
{
	this->chromozome = std::move(c.chromozome);
	this->creatureGraphBin = std::move(c.creatureGraphBin);
	this->configuration = c.configuration;
	this->fitness = c.fitness;
	    
	return *this;
}

//------------------------------------------------------------
bool Creature::operator== (const Creature& b)
{
	for (int i = 0; i < chromozome.size(); i++)
	{
		if (chromozome[i] != b.chromozome[i])
		{
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------
int Creature::calculateFittness()
{
	return creatureGraphBin.cellsFilled;
}	
//------------------------------------------------------------
Chromozome Creature::createRandomChromozome(Configuration& configuration)
{ 
	Chromozome randomChromozome;
	randomChromozome.reserve(configuration.numberOfItems);
	for (int i = 0; i < configuration.numberOfItems; i++)
	{
		randomChromozome.push_back(i);
	}
	std::random_shuffle(randomChromozome.begin(), randomChromozome.end());
        return randomChromozome;
}
//------------------------------------------------------------
