#pragma once
#include "../includes/structs.h"
#include "Configuration.h"
#include "GraphBin.h"
#include <vector>

template <class Encoding>
class CreatureNew
{
public:
	CreatureNew(Configuration* config); //constructor that creates a random creature based on the configuration;
	CreatureNew(Encoding& encoding, Configuration* config); //constructor that creates a creature based of a premade encoding
    CreatureNew(Encoding&& encoding, Configuration* config); //constructor that creates a creature based of a premade encoding
    CreatureNew(const CreatureNew& copy);
    CreatureNew(CreatureNew&& move);
	~CreatureNew();
        
    
	CreatureNew& operator=(const CreatureNew& c);
	CreatureNew& operator=(CreatureNew&& c);
        
	bool operator== (const CreatureNew& b);
    
    //functions:
    int getFittness();
    std::vector<BoxInfo> getboxesPositions();
    
    
private: 
    int fitness;
    Encoding encoding;

};


