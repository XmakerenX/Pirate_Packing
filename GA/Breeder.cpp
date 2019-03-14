#include "Breeder.h"
#include <numeric>
#include <set>
#include <algorithm>
#include "GA_Random.h"

//------------------------------------------------------------------------------------------
// input: a pair of parents and the population vector
// output children created by crossovering the parents inserted to the population vector


/*
void Breeder::mateParents(Creature& parent1, Creature & parent2, std::vector<Creature>& population)
{
	Configuration* conf = parent1.getConfiguration();
	//todo: mate parents and push the children into the population
}
//------------------------------------------------------------------------------------------
std::vector<Creature> Breeder::generateNextGeneration(std::vector<Creature> &currentPopulation)
{
	/*
	// set the probabilities for each Creature to be chosen
	std::vector<int> fitness;
	fitness.reserve(currentPopulation.size());
	for (unsigned int i = 0; i < currentPopulation.size(); i++)
	{
		fitness.push_back(currentPopulation[i].fitness);
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
		                
		mateParents(currentPopulation[parent1Index], currentPopulation[parent2Index], newPopulation);
	}
	
    std::sort(currentPopulation.begin(),currentPopulation.end(), [](const Creature& a, const Creature& b){return (a.fitness > b.fitness);});
	for (int i = 0; i < 5; i++)
		newPopulation.push_back(currentPopulation[i]);
	        
	return newPopulation;
}

//------------------------------------------------------------------------------------------
// input: a hash table with the proper replacement for each value, value that need to be replaced
// output the replacement vallue for valueToSwap
int Breeder::swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap)
{
	int newValue = hash[valueToSwap];
	// make sure new value not needed to swaped as well
	// if it is keep searching till you reach the final value
	while (hash.count(newValue) != 0)
		newValue = hash[newValue];

	return newValue;
}
//------------------------------------------------------------------------------------------
void Breeder::createChildren(Creature& parent1, Creature& parent2, int PMX_StartIndex, int PMX_EndIndex, std::vector<Creature>& population)
{
	int max, min;
	if (PMX_StartIndex > PMX_EndIndex)
	{
		max = PMX_StartIndex;
		min = PMX_EndIndex;
	}
	else
	{
		min = PMX_StartIndex;
		max = PMX_EndIndex;
	}

	Chromozome child1, child2;
	child1.reserve(parent1.chromozome.size());
	child2.reserve(parent2.chromozome.size());

	std::unordered_map<int, int> child1Hash, child2Hash;
	for (int i = min; i < max; i++)
	{
		if (parent1.chromozome[i] == parent2.chromozome[i])
			continue;

		child1Hash[parent2.chromozome[i]] = parent1.chromozome[i];
		child2Hash[parent1.chromozome[i]] = parent2.chromozome[i];
	}

	for (int i = 0; i < min; i++)
	{
		if (child1Hash.count(parent1.chromozome[i]) == 0)
			child1.push_back(parent1.chromozome[i]);
		else
			child1.push_back(swapRepetition(child1Hash, parent1.chromozome[i]));

		if (child2Hash.count(parent2.chromozome[i]) == 0)
			child2.push_back(parent2.chromozome[i]);
		else
			child2.push_back(swapRepetition(child2Hash, parent2.chromozome[i]));
	}

	for (int i = min; i < max; i++)
	{
		child1.push_back(parent2.chromozome[i]);
		child2.push_back(parent1.chromozome[i]);
	}

	for (int i = max; i < parent1.chromozome.size(); i++)
	{
		if (child1Hash.count(parent1.chromozome[i]) == 0)
			child1.push_back(parent1.chromozome[i]);
		else
			child1.push_back(swapRepetition(child1Hash, parent1.chromozome[i]));

		if (child2Hash.count(parent2.chromozome[i]) == 0)
			child2.push_back(parent2.chromozome[i]);
		else
			child2.push_back(swapRepetition(child2Hash, parent2.chromozome[i]));
	}

	std::uniform_real_distribution<> mutationChanceDist(0.0, 1.0);
	double MutationRate = 0.2;
	double randomChance = mutationChanceDist(Random::default_engine.getGenerator());
	if (randomChance < MutationRate)
		mutateChild(child1);
	randomChance = mutationChanceDist(Random::default_engine.getGenerator());
	if (randomChance < MutationRate)
		mutateChild(child2);

	population.emplace_back(child1, parent1.configuration);
	population.emplace_back(child2, parent1.configuration);
}
//------------------------------------------------------------------------------------------
void Breeder::mutateChild(Chromozome& chromozome)
{
	const int numberOfItems = chromozome.size();
	std::uniform_int_distribution<int> cromozomesIndexes(0, numberOfItems - 1);
	int gene_1;
	int gene_2;
	//get the first index
	gene_1 = cromozomesIndexes(Random::default_engine.getGenerator());

	//get the second unique index
	do
	{
		gene_2 = cromozomesIndexes(Random::default_engine.getGenerator());
	} while (gene_1 == gene_2);

	//switch indexes:
	int temp = chromozome[gene_1];
	chromozome[gene_1] = chromozome[gene_2];
	chromozome[gene_2] = temp;
}
*/