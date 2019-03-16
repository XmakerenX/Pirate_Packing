#include "Breeder.h"

//------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>&currentPopulation)
{
	//create roulette wheel to choose parents
	//Note: the Probabilities for each Creature to be chosen in the
	//      roulette are chsoen in corelation with his fittness
	std::discrete_distribution<int> roulette = createSelectionRoulette(currentPopulation);	

	//create next generation population
	float mutationChance = 0.2;
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
	for (int i = 0; i < 5; i++)
		newPopulation.push_back(currentPopulation[i]);

	return newPopulation;
}
//------------------------------------------------------------------------------------

template <class Creature>
std::discrete_distribution<int> Breeder<Creature>::createSelectionRoulette(std::vector<Creature>& currentPopulation)
{
	//normalize populationFittnesses
	normalizePopulationFittnesses(currentPopulation);

	std::vector<int> fitness;
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
// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class Breeder<BinaryCreature>;
template class Breeder<PermutationCreature>;
