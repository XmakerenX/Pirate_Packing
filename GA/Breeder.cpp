#include "Breeder.h"
#include <algorithm>

//------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>&currentPopulation)
{
	//create roulette wheel to choose parents
	//Note: the Probabilities for each Creature to be chosen in the
	//      roulette are chsoen in corelation with his fittness
	//std::discrete_distribution<int> roulette = createFairFittnesesForRouletteSelection(currentPopulation);
	//std::discrete_distribution<int> roulette = createSelectionRoulette(currentPopulation);




	std::discrete_distribution<int> roulette = createNitchingRoulette(currentPopulation);
	//create next generation population
	float mutationChance = GA_Settings::mutationRate;
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

	calculateSharedFitness(newPopulation);
	std::sort(currentPopulation.begin(), currentPopulation.end(), [](const Creature& a, const Creature& b) {return (a.getSharedFitness() > b.getSharedFitness()); });
	int elitismGroupSize = currentPopulationSize * (GA_Settings::elitismSizeGroup / 100.0f);
	for (int i = 0; i < elitismGroupSize; i++)
		newPopulation.push_back(currentPopulation[i]);




	numberOfNiches(newPopulation);
	return newPopulation;
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::calculateSharedFitness(std::vector<Creature>& currentPopulation)
{
	double minDist = 30;     double shareParam = 60;
	double denominator = 1;
	int popSize = currentPopulation.size();

	double avgHammingDistance = 0;
	for (int i = 0; i < popSize; i++)
	{
		for (int j = 0; j < popSize; j++)
		{
			//calculate the distance between those two creatures to determine if they are in the same niche
			const double dist = currentPopulation[i].hammingDistance(currentPopulation[j]);
			avgHammingDistance += dist;
			//if the two creatures are in the same niche
			if (dist < minDist)
			{
				denominator += 1 - ((double)dist / (double)30);
			}
		}
		currentPopulation[i].setSharedFitness(currentPopulation[i].getFitness() / denominator);
		assert(currentPopulation[i].getFitness() < currentPopulation[i].getSharedFitness());
	}
	avgHammingDistance = avgHammingDistance / (popSize*popSize);
	std::cout << "\nthe avarage hamming distance is:" << (int)avgHammingDistance;
}
//----------------------------------------------------------------------------------------------------
template <class Creature>
std::discrete_distribution<int>  Breeder<Creature>::createNitchingRoulette(std::vector<Creature>& currentPopulation)
{
	calculateSharedFitness(currentPopulation); //this calculate the shared fitnessValue for all of the population
	std::vector<int> sharedFitnesses;
	sharedFitnesses.reserve(currentPopulation.size());
	for (Creature creature : currentPopulation)
	{
		sharedFitnesses.push_back(creature.getSharedFitness());
	}
	std::discrete_distribution<int> roulette(sharedFitnesses.begin(), sharedFitnesses.end());
	return roulette;
}
//------------------------------------------------------------------------------------
template <class Creature>
void  Breeder<Creature>::numberOfNiches(std::vector<Creature> currentPopulation) 
{
	int numberOfNicheFound = 0;
	int maxDist = 30;

	int popSize = currentPopulation.size();
	for (int i = 0; i < popSize; i++)
	{
		if (currentPopulation[i].getFitness() > 0)
		{
			numberOfNicheFound++;
			for (int j = i + 1; j < popSize; j++)
			{
				if (  (currentPopulation[i].getFitness() > 0) && (currentPopulation[i].hammingDistance(currentPopulation[j]) <maxDist)  )
				{
					currentPopulation[j].setFitness(0);

				}
				
			}
			currentPopulation[i].setFitness(0);
		}
	}
	std::cout<<"\n" << numberOfNicheFound << " niches have been found in this population\n";
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
