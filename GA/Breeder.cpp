#include "Breeder.h"
#include <algorithm>
#include <sstream>

//-----------------------------------------------------------------------------------------------
// Name : generateNextGeneration
// Input: currentPopulation - the population from whom to create new generation
//        settings -  the GA algorithm settings
//        randomEngine - the random number generator to use for random numbers
// Output: The next generation population
// Action: Creates the next generation population from the current population
//-----------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> Breeder<Creature>::generateNextGeneration(std::vector<Creature>& currentPopulation, const GA_Settings& settings,
                                                                Random& randomEngine/* = Random::default_engine*/)
{
	//create new generation vector:
	std::vector<Creature> newPopulation;
	newPopulation.reserve(currentPopulation.size() * 2);

	std::cout << "\nNumber of that niches found:" << calcualteNiches(currentPopulation) << "\n";
        
	if (settings.multiThread)
		generateNextGenerationMultiThread(currentPopulation, newPopulation, settings);
	else
		generateNextGenerationSingleThread(currentPopulation, newPopulation, settings, randomEngine);

	if (settings.nitchingEnabled)
		calculateSharedFitness(newPopulation, settings.multiThread);

	return newPopulation;
}

//-----------------------------------------------------------------------------------------------
// Name :  generateNextGenerationSingleThread
// Input:  currentPopulation - the population from whom to create new generation
//         settings -  the GA algorithm settings
//         randomEngine   - the random number generator to use for random numbers
// Output: newPopulation - The next generation population
// Action: Creates the next generation population from the current population using a single thread
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::generateNextGenerationSingleThread(std::vector<Creature>& currentPopulation, std::vector<Creature>& newPopulation, const GA_Settings& settings, Random& randomEngine)
{
	std::discrete_distribution<int> roulette = createRoulette(currentPopulation, settings.nitchingEnabled);
	float mutationChance = settings.mutationRate;
	int currentPopulationSize = currentPopulation.size();
	for (int i = 0; i < currentPopulationSize; i++)
	{
		int parent1Index = roulette(randomEngine.getGenerator());
		int parent2Index;
		do
		{
			parent2Index = roulette(randomEngine.getGenerator());
		} while (parent2Index == parent1Index);

		currentPopulation[parent1Index].crossover(currentPopulation[parent2Index], newPopulation, randomEngine);
		newPopulation[newPopulation.size() - 1].mutate(mutationChance, randomEngine);
		newPopulation[newPopulation.size() - 1].calculateFittness();
		newPopulation[newPopulation.size() - 2].mutate(mutationChance, randomEngine);
		newPopulation[newPopulation.size() - 2].calculateFittness();
	}
}

//-----------------------------------------------------------------------------------------------
// Name :  generateNextGenerationMultiThread
// Input:  currentPopulation - the population from whom to create new generation
//         settings -  the GA algorithm settings
// Output: newPopulation - The next generation population
// Action: Creates the next generation population from the current population using a multiple threads
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::generateNextGenerationMultiThread(std::vector<Creature>& currentPopulation, std::vector<Creature>& newPopulation, const GA_Settings& settings)
{
	std::discrete_distribution<int> roulette = createRoulette(currentPopulation, settings.nitchingEnabled);
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
	std::thread SemiBreeder1(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder1), currentPopulationSize / 4, settings);
	std::thread SemiBreeder2(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder2), currentPopulationSize / 4, settings);
	std::thread SemiBreeder3(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder3), currentPopulationSize / 4, settings);
	std::thread SemiBreeder4(semiBreeder, currentPopulation, roulette, std::move(promoiseSemiBreeder4), currentPopulationSize - 3 * (currentPopulationSize / 4), settings);

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

//-----------------------------------------------------------------------------------------------
// Name :  semiBreeder
// Input:  currentPopulation - the population from whom to create new generation
//         roulette -  use this roulette to select the parents
//         numberOfParentsPair - how many parents to breed
//         settings - the GA algorithm settings
// Output: creaturesCreated - the part of the new generation created by the semiBreeder
// Action: breeds parents based on numberOfParentsPair and output the children to creaturesCreated
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::semiBreeder(const std::vector<Creature>& currentPopulation,std::discrete_distribution<int> roulette,
                                    std::promise<std::vector<Creature>>&& creaturesCreated,int numberOfParentsPair, const GA_Settings& settings)
{
	unsigned long int seed;
	// thread id is only obtainable by printing it...
	// so print it to a temp stream and convert it back to int
	std::stringstream stream;
	stream << std::this_thread::get_id();
	stream >> seed;
	Random randomEngine(seed);

	std::vector<Creature> newCreatures;
	newCreatures.reserve(numberOfParentsPair * 2);
	float mutationChance = settings.mutationRate;
	for (int i = 0; i < numberOfParentsPair; i++)
	{
		int parent1Index;
		int parent2Index;
		chooseParents(parent1Index, parent2Index, roulette, randomEngine);
		currentPopulation[parent1Index].crossover(currentPopulation[parent2Index], newCreatures, randomEngine);
		int creaturesCreatedSoFar = (newCreatures).size();
		      newCreatures[creaturesCreatedSoFar - 1].mutate(mutationChance, randomEngine);
		      newCreatures[creaturesCreatedSoFar - 1].calculateFittness();
		      newCreatures[creaturesCreatedSoFar - 2].mutate(mutationChance, randomEngine);
		      newCreatures[creaturesCreatedSoFar - 2].calculateFittness();
	}
	
	creaturesCreated.set_value(newCreatures);
}

//-----------------------------------------------------------------------------------------------
// Name :  chooseParents
// Input:  currentPopulation - the population from whom to create new generation
//         roulette -  use this roulette to select the parents
//         randomEngine - the random number generator to use for random numbers
// Output: parent1Index - the first parent index
//         parent2Index - the second parent index
// Action: select 2 index for parents based on the roulette
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::chooseParents(int& parent1Index, int& parent2Index, std::discrete_distribution<int>& roulette, Random& randomEngine)
{
	//choose one parent:
	parent1Index = roulette(randomEngine.getGenerator());
	//choose second parent:
	do
	{
		parent2Index = roulette(randomEngine.getGenerator());
	} 
	while (parent2Index == parent1Index);
}

//-----------------------------------------------------------------------------------------------
// Name :  createRoulette
// Input:  currentPopulation - the population from whom to create new generation
//         nitchingEnabled -  state if to use shared fitness or not
// Output: a roulette to select pair of parents
// Action: create a roulette based on the population and if shared fitness is enabled or not
//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
// Name :  createSelectionRoulette
// Input:  currentPopulation - the population from whom to create new generation
// Output: a roulette to select pair of parents
// Action: create a roulette based on the population where the fitter you are the more likly you will be choosen
//-----------------------------------------------------------------------------------------------
template <class Creature>
std::discrete_distribution<int> Breeder<Creature>::createSelectionRoulette(std::vector<Creature>& currentPopulation)
{
	//normalize populationFittnesses
	std::vector<int> fitness = normalizePopulationFittnesses(currentPopulation);
	// roulette will choose random pop based on the probabilities vector 
	std::discrete_distribution<int> roulette(fitness.begin(), fitness.end());
	return roulette;
}

//-----------------------------------------------------------------------------------------------
// Name :  normalizePopulationFittnesses
// Input:  currentPopulation - the population from whom to create new generation
// Output: population with non negative fitness 
// Action: normalize all fitness value to positve values so a roulette can be created
//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
// Name :  createSelectionRoulette
// Input:  currentPopulation - the population from whom to create new generation
// Output: a roulette to select pair of parents
// Action: create a roulette based on the population where parent selection will more diverse and from different niches
//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
// Name :  calculateSharedFitness
// Input:  currentPopulation - the population from whom to create new generation
//         multiThread = states if to run the calcuation multiThreaded or not
// Output: sets the shread fitness for the population creatures
// Action: calculate the shared fitness for the creautres in the population
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::calculateSharedFitness(std::vector<Creature>& currentPopulation, bool multiThread/* = true*/)
{
	if (multiThread)
		calculateSharedFitnessMultiThread(currentPopulation);
	else
		calculateSharedFitnessSingleThread(currentPopulation);
}

//-----------------------------------------------------------------------------------------------
// Name :  calculateSharedFitnessSingleThread
// Input:  currentPopulation - the population from whom to create new generation
// Output: sets the shread fitness for the population creatures
// Action: calculate the shared fitness for the creautres in the population in a single thread
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::calculateSharedFitnessSingleThread(std::vector<Creature>& currentPopulation)
{
	//parameters:
	int minDistance = currentPopulation[0].getMinDist();
	std::vector<double> starvationFactor(currentPopulation.size(), 1);

	int populationSize = currentPopulation.size();
	//for each creature in the population
	for (int i = 0; i < populationSize; i++)
	{
		//double starvationFactor = 1;
		for (int j = i + 1; j < populationSize; j++)
		{
			//calculate current distance
			int distance = currentPopulation[i].hammingDistance(currentPopulation[j]);
			if (distance < minDistance)
			{
				//increase the starvation factor for each creature that is close to the current one
				starvationFactor[i] += (1 - ((double)distance / minDistance));
                                starvationFactor[j] += (1 - ((double)distance / minDistance));
			}
		}
		double sharedFitness = currentPopulation[i].getFitness() / starvationFactor[i];
		currentPopulation[i].setSharedFitness(sharedFitness);
	}
}

//-----------------------------------------------------------------------------------------------
// Name :  calculateSharedFitnessMultiThread
// Input:  currentPopulation - the population from whom to create new generation
// Output: sets the shread fitness for the population creatures
// Action: calculate the shared fitness for the creautres in the population in a multiple threads
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::calculateSharedFitnessMultiThread(std::vector<Creature>& currentPopulation)
{
	int indexDelta = currentPopulation.size() / 4;
	std::thread SemiSharedFitness1(semiCalculateSharedFitness, std::ref(currentPopulation), 0, indexDelta);
	std::thread SemiSharedFitness2(semiCalculateSharedFitness, std::ref(currentPopulation), indexDelta, indexDelta*2);
	std::thread SemiSharedFitness3(semiCalculateSharedFitness, std::ref(currentPopulation), indexDelta*2, indexDelta*3);
	std::thread SemiSharedFitness4(semiCalculateSharedFitness, std::ref(currentPopulation), indexDelta*3, currentPopulation.size());
        
	SemiSharedFitness1.join();
	SemiSharedFitness2.join();
	SemiSharedFitness3.join();
	SemiSharedFitness4.join();
}

//-----------------------------------------------------------------------------------------------
// Name :  calculateSharedFitnessMultiThread
// Input:  currentPopulation - the population from whom to create new generation
//         startIndex - the index from each to start calculating
//         endIndex - the index to stop calculating
// Output: sets the shread fitness for the population creatures in the given index range
// Action: calculate the shared fitness for the creautres in the population in the given index range
//-----------------------------------------------------------------------------------------------
template <class Creature>
void Breeder<Creature>::semiCalculateSharedFitness(std::vector<Creature>& currentPopulation,int startIndex , int endIndex)
{
	int minDistance = currentPopulation[0].getMinDist();

	int populationSize = currentPopulation.size();
	//for each creature in the population
	for (int i = startIndex; i < endIndex; i++)
	{
		double starvationFactor = 1;
		//double starvationFactor = 1;
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
}

//-----------------------------------------------------------------------------------------------
// Name :  calcualteNiches
// Input:  currentPopulation - the population from whom to create new generation
// Output: The number of niches in the population
// Action: calculate The number of niches in the population
//-----------------------------------------------------------------------------------------------
template <class Creature>
int  Breeder<Creature>::calcualteNiches(std::vector<Creature>&currentPopulation)
{
	int populationSize = currentPopulation.size();
	int minDistance = currentPopulation[0].getMinDist();
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
	
	return numberOfNiches;
}

//------------------------------------------------------------------------------------
// Force instantiation of Breeder<BinaryCreature> and Breeder<PermutationCreature>
//------------------------------------------------------------------------------------
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class Breeder<BinaryCreature>;
template class Breeder<PermutationCreature>;
