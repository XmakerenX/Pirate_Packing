#include "Breeder.h"
#include <numeric>
#include <set>
#include <algorithm>
#include <limits>
#include "GA_Random.h"

//------------------------------------------------------------------------------------------
template <class encoding>
std::vector<Creature<encoding>> Breeder<encoding>::generateNextGeneration(std::vector<Creature<encoding>> &currentPopulation)
{
    float mutationChance = 0.2;
    int min = std::numeric_limits<int>::max();
    for (int i = 0; i < currentPopulation.size(); i++)
    {
        if (currentPopulation[i].fitness < min)
            min = currentPopulation[i].fitness;
    }
    
    if (min < 0)
    {
        for (int i = 0; i < currentPopulation.size(); i++)
            currentPopulation[i].fitness -= (min + 1);
    }
    
	// set the probabilities for each Creature to be chosen
	std::vector<int> fitness;
	fitness.reserve(currentPopulation.size());
	for (unsigned int i = 0; i < currentPopulation.size(); i++)
	{
		fitness.push_back(currentPopulation[i].fitness);
	}
	// roulette will choose random pop based on the probabilities vector 
	std::discrete_distribution<int> roulette(fitness.begin(), fitness.end());
	
	std::vector<Creature<encoding>> newPopulation;
	int currentPopulationSize = currentPopulation.size();
	newPopulation.reserve(currentPopulationSize*2);
	
	for (int i = 0; i < currentPopulationSize; i++)
	{
		int parent1Index = roulette(Random::default_engine.getGenerator());
		int parent2Index;
		do { parent2Index = roulette(Random::default_engine.getGenerator()); }
		while (parent2Index == parent1Index);
		            
        currentPopulation[parent1Index].mate(currentPopulation[parent2Index], newPopulation);
        newPopulation[newPopulation.size() - 1].mutate(mutationChance);
        newPopulation[newPopulation.size() - 1].updateFitness();
        newPopulation[newPopulation.size() - 2].mutate(mutationChance);
        newPopulation[newPopulation.size() - 2].updateFitness();
	}
	
    std::sort(currentPopulation.begin(),currentPopulation.end(), [](const Creature<encoding>& a, const Creature<encoding>& b){return (a.fitness > b.fitness);});
	for (int i = 0; i < 5; i++)
		newPopulation.push_back(currentPopulation[i]);
	        
	return newPopulation;
}
