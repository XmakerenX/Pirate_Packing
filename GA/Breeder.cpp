#include "Breeder.h"
#include <algorithm>

//------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>&currentPopulation, const GA_Settings& settings)
{
	//create roulette wheel to choose parents
	//Note: the Probabilities for each Creature to be chosen in the
	//      roulette are chsoen in corelation with his fittness
	//std::discrete_distribution<int> roulette = createFairFittnesesForRouletteSelection(currentPopulation);
	std::discrete_distribution<int> roulette = createSelectionRoulette(currentPopulation);

	//create next generation population
	float mutationChance = settings.mutationRate;
	int currentPopulationSize = currentPopulation.size();
	std::vector<Creature> newPopulation;
	newPopulation.reserve(currentPopulationSize * 2);

	for (int i = 0; i < currentPopulationSize; i++)
	{
		int parent1Index = roulette(Random::default_engine.getGenerator());
		int parent2Index;
		do
		{
			parent2Index = roulette(Random::default_engine.getGenerator());
		} while (parent2Index == parent1Index);

		currentPopulation[parent1Index].crossover(currentPopulation[parent2Index], newPopulation);
		newPopulation[newPopulation.size() - 1].mutate(mutationChance);
		newPopulation[newPopulation.size() - 1].calculateFittness();
		newPopulation[newPopulation.size() - 2].mutate(mutationChance);
		newPopulation[newPopulation.size() - 2].calculateFittness();
	}

	std::sort(currentPopulation.begin(), currentPopulation.end(), [](const Creature& a, const Creature& b) {return (a.getFitness() > b.getFitness()); });
	int elitismGroupSize = currentPopulationSize * (settings.elitismSizeGroup / 100.0f);
	for (int i = 0; i < elitismGroupSize; i++)
		newPopulation.push_back(currentPopulation[i]);

	return newPopulation;
}
//------------------------------------------------------------------------------------

template <class Creature>
std::discrete_distribution<int> Breeder<Creature>::createSelectionRoulette(std::vector<Creature>& currentPopulation)
{
	//normalize populationFittnesses
	std::vector<int> fitness = normalizePopulationFittnesses(currentPopulation);
	// roulette will choose random pop based on the probabilities vector 
	std::discrete_distribution<int> roulette(fitness.begin(), fitness.end());
	return roulette;
}
//------------------------------------------------------------------------------------
template <class Creature>
std::vector<int>  Breeder<Creature>::normalizePopulationFittnesses(std::vector<Creature>& currentPopulation)
{
    std::vector<int> normalizedFitness;
    normalizedFitness.reserve(currentPopulation.size());
	int min = std::numeric_limits<int>::max();
	//get minimum fittness in population
	for (Creature& creature : currentPopulation)
	{
		normalizedFitness.push_back(creature.getFitness());
		min = std::min(min, creature.getFitness());
	}

	if (min < 0)
	{
		//normalize fittness 
		for (int& fitness : normalizedFitness)
			fitness -= (min - 1);
	}
	
	return normalizedFitness;
}
//------------------------------------------------------------------------------------
//While applying the GA algorithm, one might want to consider the fact that weak creatures (realitve to their generation)
//may have, despite being consider weak, a good attribute that could amend the gene pool, this method constructs a fair roulette
//which gives a low, yet fair cahnce for 
template <class Creature>
std::discrete_distribution<int>  Breeder<Creature>::createFairFittnesesForRouletteSelection(std::vector<Creature>& currentPopulation)
{
	normalizePopulationFittnesses(currentPopulation);
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::max();

	//get minimum fittness in population
	for (Creature& creature : currentPopulation)
	{
		min = std::min(min, creature.getFitness());
		max = std::max(max, creature.getFitness());
	}
	std::vector<int> fitness;
	fitness.reserve(currentPopulation.size());
	for (Creature& creature : currentPopulation)
	{
		int score = creature.getFitness() - min;
		score += (max - min) / 3;
		fitness.push_back(creature.getFitness());
	}
	// roulette will choose random pop based on the probabilities vector 
	std::discrete_distribution<int> roulette(fitness.begin(), fitness.end());
	return roulette;
}
//------------------------------------------------------------------------------------
// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class Breeder<BinaryCreature>;
template class Breeder<PermutationCreature>;
