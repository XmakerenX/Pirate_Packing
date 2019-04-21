#include "Breeder.h"
#include <algorithm>

//------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>&currentPopulation, bool multiThread/* = true*/)
{
	//create new generation vector:
	std::vector<Creature> newPopulation;
	newPopulation.reserve(currentPopulation.size() * 2);
	int currentPopulationSize = currentPopulation.size();

	if (multiThread)
		generateNextGenerationMultiThread(currentPopulation, newPopulation);
	else
		generateNextGenerationSingleThread(currentPopulation, newPopulation);

	if (GA_Settings::nitchingEnabled)
	{
		calculateSharedFitness(newPopulation);
		//sort the population by their share fitness
		std::sort(currentPopulation.begin(), currentPopulation.end(), 
			[](const Creature& a, const Creature& b){
			return (a.getSharedFitness() > b.getSharedFitness());
		});
	}
	else
	{
		std::sort(currentPopulation.begin(), currentPopulation.end(),
			[](const Creature& a, const Creature& b) {return (a.getFitness() > b.getFitness()); });
	}
	
	int elitismGroupSize = currentPopulationSize * (GA_Settings::elitismSizeGroup / 100.0f);
	for (int i = 0; i < elitismGroupSize; i++)
		newPopulation.push_back(currentPopulation[i]);

	return newPopulation;
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::generateNextGenerationSingleThread(std::vector<Creature>& currentPopulation, std::vector<Creature>& newPopulation)
{
	std::discrete_distribution<int> roulette = createRoulette(currentPopulation, GA_Settings::nitchingEnabled);
	float mutationChance = GA_Settings::mutationRate;
	int currentPopulationSize = currentPopulation.size();
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
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::generateNextGenerationMultiThread(std::vector<Creature>& currentPopulation, std::vector<Creature>& newPopulation)
{
	std::discrete_distribution<int> roulette = createRoulette(currentPopulation, GA_Settings::nitchingEnabled);
	//divide the breeding job to 4 differents semi-breeders
	int currentPopulationSize = currentPopulation.size();
	std::promise<std::vector<Creature>> promoiseSemiBreeder1;
	std::future<std::vector<Creature>> retSemiBreeder1 = promoiseSemiBreeder1.get_future();
	std::promise<std::vector<Creature>> promoiseSemiBreeder2;
	std::future<std::vector<Creature>> retSemiBreeder2 = promoiseSemiBreeder2.get_future();
	std::promise<std::vector<Creature>> promoiseSemiBreeder3;
	std::future<std::vector<Creature>> retSemiBreeder3 = promoiseSemiBreeder3.get_future();
	std::promise<std::vector<Creature>> promoiseSemiBreeder4;
	std::future<std::vector<Creature>> retSemiBreeder4 = promoiseSemiBreeder4.get_future();
	//create the 4 SemiBreeders
	std::thread SemiBreeder1(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder1), currentPopulationSize / 4);
	std::thread SemiBreeder2(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder2), currentPopulationSize / 4);
	std::thread SemiBreeder3(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder3), currentPopulationSize / 4);
	std::thread SemiBreeder4(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder4), currentPopulationSize / 4);

	//wait for all the SemiBreeders to finish
	SemiBreeder1.join();
	SemiBreeder2.join();
	SemiBreeder3.join();
	SemiBreeder4.join();
        
	std::vector<Creature> SemiPopulation1 = retSemiBreeder1.get(); 
	std::vector<Creature> SemiPopulation2 = retSemiBreeder2.get(); 
	std::vector<Creature> SemiPopulation3 = retSemiBreeder3.get(); 
	std::vector<Creature> SemiPopulation4 = retSemiBreeder4.get();

	for (Creature& creature : SemiPopulation1) newPopulation.emplace_back(std::move(creature));
	for (Creature& creature : SemiPopulation2) newPopulation.emplace_back(std::move(creature));
	for (Creature& creature : SemiPopulation3) newPopulation.emplace_back(std::move(creature));
	for (Creature& creature : SemiPopulation4) newPopulation.emplace_back(std::move(creature));
}

//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::semiBreeder(const std::vector<Creature>& currentPopulation,std::discrete_distribution<int> roulette,
                                    std::promise<std::vector<Creature>>&& creaturesCreated,int numberOfParentsPair)
{
	std::vector<Creature> newCreatures;
	newCreatures.reserve(numberOfParentsPair * 2);
	float mutationChance = GA_Settings::mutationRate;
	for (int i = 0; i < numberOfParentsPair; i++)
	{
		int parent1Index;
		int parent2Index;
		chooseParents(parent1Index, parent2Index, roulette);
		currentPopulation[parent1Index].crossover(currentPopulation[parent2Index], newCreatures);
		int creaturesCreatedSoFar = (newCreatures).size();
		      newCreatures[creaturesCreatedSoFar - 1].mutate(mutationChance);
		      newCreatures[creaturesCreatedSoFar - 1].calculateFittness();
		      newCreatures[creaturesCreatedSoFar - 2].mutate(mutationChance);
		      newCreatures[creaturesCreatedSoFar - 2].calculateFittness();
	}
	
	creaturesCreated.set_value(newCreatures);
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::chooseParents(int& parent1Index, int& parent2Index, std::discrete_distribution<int> roulette)
{
	//choose one parent:
	parent1Index = roulette(Random::default_engine.getGenerator());
	//choose second parent:
	do
	{
		parent2Index = roulette(Random::default_engine.getGenerator());
	} 
	while (parent2Index == parent1Index);
}
//------------------------------------------------------------------------------------
template <class Creature>
std::discrete_distribution<int> Breeder<Creature>::createRoulette(std::vector<Creature>& currentPopulation, bool nitchingEnabled)
{
	//create roulette wheel to choose parents
	//Note: the Probabilities for each Creature to be chosen in the roulette are chsoen in corelation with his fittness
	if(nitchingEnabled) 
		return createNitchingRoulette(currentPopulation);
	else                             
		return createSelectionRoulette(currentPopulation);
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
//----------------------------------------------------------------------------------------------------------------
template <class Creature>
std::discrete_distribution<int> Breeder<Creature>::createNitchingRoulette(std::vector<Creature>& currentPopulation)
{
	calculateSharedFitness(currentPopulation);

	std::vector<int> sharedFitnesses;
	sharedFitnesses.reserve(currentPopulation.size());

	for (Creature creautre : currentPopulation)
	{
		sharedFitnesses.push_back(creautre.getSharedFitness());
	}
	std::discrete_distribution<int> roulette(sharedFitnesses.begin(), sharedFitnesses.end());
	return roulette;
}
//------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::calculateSharedFitness(std::vector<Creature>& currentPopulation)
{
	//parameters:
	int minDistance = 30;

	int populationSize = currentPopulation.size();
	//for each creature in the population
	for (int i = 0;i<populationSize; i++)
	{
		double starvationFactor = 1;
		for (int j = 0; j < populationSize; j++)
		{
			//calculate current distance
			int distance = currentPopulation[i].hammingDistance(currentPopulation[j]);
			if (distance < minDistance)
			{
				//increase the starvation factor for each creature that is close to the current one
				starvationFactor += (1 - ((double)distance / minDistance));
			}
		}
		double sharedFitness = currentPopulation[i].getFitness() / starvationFactor;
		currentPopulation[i].setSharedFitness(sharedFitness);
	}

	std::vector<int> creatures;
	for (int i = 0; i < populationSize; i++) creatures.push_back(1);
	int numberOfNiches = 0;
	for (int i = 0; i < populationSize; i++)
	{
		if (creatures[i] != -1)
		{
			creatures[i] = -1;
			for (int j = i + 1; j < populationSize; j++)
			{
				if (currentPopulation[i].hammingDistance(currentPopulation[j]) < minDistance)
				{
					creatures[j] = -1;
				}
			}
			numberOfNiches++;
		}
	}
	std::cout << "\nNumber of that niches found:" << numberOfNiches << "\n";
}
//------------------------------------------------------------------------------------
// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class Breeder<BinaryCreature>;
template class Breeder<PermutationCreature>;
