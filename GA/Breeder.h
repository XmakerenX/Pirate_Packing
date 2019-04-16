#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include "GA_Random.h"
#include "GA_Settings.h"

template <class Creature>
class Breeder
{
public:
	static std::vector<Creature> generateNextGeneration(std::vector<Creature>& currentPopulation);
	static std::vector<Creature> sharedFitnessPopulation;


private:
	static std::vector<int>  normalizePopulationFittnesses(std::vector<Creature>& currentPopulation);
	static std::discrete_distribution<int> createFairFittnesesForRouletteSelection(std::vector<Creature>& currentPopulation);
	static std::discrete_distribution<int> createSelectionRoulette(std::vector<Creature>& currentPopulation);
	static std::discrete_distribution<int> createNitchingRoulette(std::vector<Creature>&currentPopulation);
	static void numberOfNiches(std::vector<Creature> currentPopulation);
	static void calculateSharedFitness(std::vector<Creature>& currentPopulation);


};
