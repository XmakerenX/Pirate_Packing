#include "Breeder.h"
#include "GA_Random.h"
#include <algorithm>    
#include <iostream>
#include <random>
#include <vector>
#include <numeric>
#include <set>
#include <algorithm>
#include <limits>

//------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>&currentPopulation)
{
	std::vector<int> fitness;
	std::discrete_distribution<int> roulette;
	
	//create roulette wheel to choose parents
	roulette = createSelectionRoulette(currentPopulation,fitness);	//Note: the Probabilities for each Creature to be chosen in the
																//roulette are chsoen in corelation with his fittness
	//create next generation population
	std::vector<Creature> newPopulation;
	createNextGenerationPopulation(currentPopulation, roulette, newPopulation);

	       
	return newPopulation;
}
//------------------------------------------------------------------------------------

template <class Creature>
std::discrete_distribution<int> Breeder<Creature>::createSelectionRoulette(std::vector<Creature>& currentPopulation, std::vector<int>& fitness)
{
	//normalize populationFittnesses
	normalizePopulationFittnesses(currentPopulation);

	fitness.reserve(currentPopulation.size());
	for (Creature& creature : currentPopulation)
	{
		fitness.push_back(creature.getFitness());
	}
	// roulette will choose random pop based on the probabilities vector 
	std::discrete_distribution<int> roulette(fitness.begin(), fitness.end());
	return roulette;
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::normalizePopulationFittnesses(std::vector<Creature>& currentPopulation)
{
	int min = std::numeric_limits<int>::max();
	//get minimum fittness in population
	for (Creature& creature : currentPopulation)  {  min = std::min(min, creature.getFitness()); }
	
	if (min < 0)  
	{
		//normalize fittness 
		for (Creature& creature : currentPopulation)
		{
			creature.setFitness(creature.getFitness() - (min + 1));
		}
	}
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::createNextGenerationPopulation(std::vector<Creature>& currentPopulation, std::discrete_distribution<int>& roulette,
	std::vector<Creature>& newPopulation)
{
	float mutationChance = 0.2;
	int currentPopulationSize = currentPopulation.size();
	newPopulation.reserve(currentPopulationSize * 2);

	for (int i = 0; i < currentPopulationSize; i++)
	{
		int parent1Index = roulette(Random::default_engine.getGenerator());
		int parent2Index;
		do { parent2Index = roulette(Random::default_engine.getGenerator()); } while (parent2Index == parent1Index);

		currentPopulation[parent1Index].crossover(currentPopulation[parent2Index], newPopulation);
		newPopulation[newPopulation.size() - 1].mutate(mutationChance);
		newPopulation[newPopulation.size() - 1].calculateFittness();
		newPopulation[newPopulation.size() - 2].mutate(mutationChance);
		newPopulation[newPopulation.size() - 2].calculateFittness();
	}

	std::sort(currentPopulation.begin(), currentPopulation.end(), [](const Creature& a, const Creature& b) {return (a.getFitness() > b.getFitness()); });
	for (int i = 0; i < 5; i++)
		newPopulation.push_back(currentPopulation[i]);
}
//------------------------------------------------------------------------------------
// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class Breeder<BinaryCreature>;
template class Breeder<PermutationCreature>;
