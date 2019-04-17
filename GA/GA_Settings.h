#pragma once
#include "../includes/structs.h"

class GA_Settings
{
public:
	GA_Settings(GA_Method _method, float _mutationRate, unsigned int _numberOfGenerations,
                    int _populationSize, unsigned int _elitismSizeGroup);
	GA_Settings(const GA_Settings& copy);
    
	GA_Method method;
	float mutationRate;
	unsigned int numberOfGenerations;
	int populationSize;
	unsigned int elitismSizeGroup;
};
