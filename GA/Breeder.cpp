#include "Breeder.h"
#include <numeric>
#include <set>
#include <algorithm>
#include <limits>
#include "GA_Random.h"

//------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>&currentPopulation)
{
    float mutationChance = 0.2;
    int min = std::numeric_limits<int>::max();
    for (int i = 0; i < currentPopulation.size(); i++)
    {
        if (currentPopulation[i].getFitness() < min)
            min = currentPopulation[i].getFitness();
    }
    
    if (min < 0)
    {
        for (int i = 0; i < currentPopulation.size(); i++)
            currentPopulation[i].setFitness(currentPopulation[i].getFitness() - (min + 1));
    }
    
	// set the probabilities for each Creature to be chosen
	std::vector<int> fitness;
	fitness.reserve(currentPopulation.size());
	for (unsigned int i = 0; i < currentPopulation.size(); i++)
	{
		fitness.push_back(currentPopulation[i].getFitness());
	}
	// roulette will choose random pop based on the probabilities vector 
	std::discrete_distribution<int> roulette(fitness.begin(), fitness.end());
	
	std::vector<Creature> newPopulation;
	int currentPopulationSize = currentPopulation.size();
	newPopulation.reserve(currentPopulationSize*2);
	
	for (int i = 0; i < currentPopulationSize; i++)
	{
		int parent1Index = roulette(Random::default_engine.getGenerator());
		int parent2Index;
		do { parent2Index = roulette(Random::default_engine.getGenerator()); }
		while (parent2Index == parent1Index);
		            
        currentPopulation[parent1Index].crossover(currentPopulation[parent2Index], newPopulation);
        newPopulation[newPopulation.size() - 1].mutate(mutationChance);
        newPopulation[newPopulation.size() - 1].calculateFittness();
        newPopulation[newPopulation.size() - 2].mutate(mutationChance);
        newPopulation[newPopulation.size() - 2].calculateFittness();
	}
	
    std::sort(currentPopulation.begin(),currentPopulation.end(), [](const Creature& a, const Creature& b){return (a.getFitness() > b.getFitness());});
	for (int i = 0; i < 5; i++)
		newPopulation.push_back(currentPopulation[i]);
	        
	return newPopulation;
}

//------------------------------------------------------------------------------------
// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class Breeder<BinaryCreature>;
template class Breeder<PermutationCreature>;
