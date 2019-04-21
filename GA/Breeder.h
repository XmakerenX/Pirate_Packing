#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include <thread>
#include <future>

#include "GA_Random.h"
#include "GA_Settings.h"

template <class Creature>
class Breeder
{
public:
	static std::vector<Creature> generateNextGeneration(std::vector<Creature>& currentPopulation);
private:
	static void semiBreeder(const std::vector<Creature>& currentPopulation,std::discrete_distribution<int> roulette,
							std::promise<std::vector<Creature>>&& creaturesCreated,int numberOfParentsPair);
	static void chooseParents(int& parent1, int& parent2,std::discrete_distribution<int> roulette);
	static std::vector<int>  normalizePopulationFittnesses(std::vector<Creature>& currentPopulation);
	static std::discrete_distribution<int> createFairFittnesesForRouletteSelection(std::vector<Creature>& currentPopulation);
	static std::discrete_distribution<int> createSelectionRoulette(std::vector<Creature>& currentPopulation);
	static void calculateSharedFitness(std::vector<Creature>& currentPopulation);
	static std::discrete_distribution<int> createNitchingRoulette(std::vector<Creature>& currentPopulation);
        
};
