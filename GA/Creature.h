#pragma once
#include "../includes/structs.h"
#include "Configuration.h"
#include "GraphBin.h"

class Creature
{
public:
	Creature(Chromozome newChromozome,Configuration* config);        
	Creature(const Creature& c);
	Creature(Creature&& move);
	~Creature();
        
	Creature& operator=(const Creature& c);
	Creature& operator=(Creature&& c);
        
	bool operator== (const Creature& b);
        
	//functions:
	int calculateFittness();
	static Chromozome createRandomChromozome(Configuration& configuration);
	Chromozome chromozome;
	Configuration* configuration;
	int fitness;
	GraphBin creatureGraphBin;
};

