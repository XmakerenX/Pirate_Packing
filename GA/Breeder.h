#pragma once
#include <vector>
#include "Creature.h"
template <class encoding>
class Breeder
{
public:
	static std::vector<encoding> generateNextGeneration(std::vector<encoding>&currentPopulation);
};
