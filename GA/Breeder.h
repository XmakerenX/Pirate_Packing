#pragma once
#include <vector>
template <class Creature>
class Breeder
{
public:
	static std::vector<Creature> generateNextGeneration(std::vector<Creature>&currentPopulation);
private:
	static void normalizePopulationFittnesses(std::vector<Creature>&currentPopulation);
	static std::discrete_distribution<int> createSelectionRoulette(std::vector<Creature>&currentPopulation, std::vector<int>& fitness);
	static void createNextGenerationPopulation(std::vector<Creature>&currentPopulation, std::discrete_distribution<int>& roulette,
											   std::vector<Creature>&newPopulation);

};
