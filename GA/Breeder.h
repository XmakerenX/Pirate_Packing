#pragma once
#include <vector>
template <class Creature>
class Breeder
{
public:
	static std::vector<Creature> generateNextGeneration(std::vector<Creature>&currentPopulation);
};
