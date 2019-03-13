#pragma once
#include "../includes/structs.h"
#include "Configuration.h"
#include <vector>

template <class Encoding>
class Creature
{
public:
	Creature(Configuration* config); //constructor that creates a random creature based on the configuration;
	Creature(Encoding& encoding, Configuration* config); //constructor that creates a creature based of a premade encoding
    Creature(Encoding&& encoding, Configuration* config); //constructor that creates a creature based of a premade encoding
    Creature(const Creature& copy);
    Creature(Creature&& move);
	~Creature();
        
    
	Creature& operator=(const Creature& c);
	Creature& operator=(Creature&& c);
        
	bool operator== (const Creature& b);
    
    //functions:
    int getFittness();
	Configuration* getConfiguration();

    std::vector<BoxInfo> getboxesPositions();
    
    
private: 
    int fitness;
    Encoding encoding;
	Configuration* configuration;

};


