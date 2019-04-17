#include "GA_Settings.h"

GA_Settings::GA_Settings(GA_Method _method, float _mutationRate, unsigned int _numberOfGenerations,
                    int _populationSize, unsigned int _elitismSizeGroup)
	 :method(_method), mutationRate(_mutationRate), numberOfGenerations(_numberOfGenerations),
	  populationSize(_populationSize), elitismSizeGroup(_elitismSizeGroup)
{
}

GA_Settings::GA_Settings(const GA_Settings& copy)
	 :method(copy.method), 
	  mutationRate(copy.mutationRate),
	  numberOfGenerations(copy.numberOfGenerations),
	  populationSize(copy.populationSize),
	  elitismSizeGroup(copy.elitismSizeGroup)
{}
