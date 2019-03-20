#pragma once
#include "../includes/structs.h"

class GA_Settings
{
	public:
		static GA_Method method;
		static float mutationRate;
		static unsigned int numberOfGenerations;
		static unsigned int populationSize;
		static unsigned int elitismSizeGroup;
};