#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"
#include <vector>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string>

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems), overallMaximumFitness(0), generationMaximumFitness(0)
{
	
}

//-----------------------------------------------------------------------------------------------
// run
//-----------------------------------------------------------------------------------------------
void GAThread::run()
{
	std::cout << "seed " << Random::default_engine.getSeed() << "\n";
	configuration.Reset();

	emit GAStarted();
	HybridGeneticAlgorithm(configuration);     //apply genetic algorithm on this configuration
	emit GAFinished();
}

//-----------------------------------------------------------------------
void GAThread::HybridGeneticAlgorithm(Configuration& configuration)
{
	overallMaximumFitness = 0;
	//----Genetic algorithm: first generation ------//
	std::vector<PermutationCreature> population = generateFirstGeneration(configuration);
 
 	//----Genetic algorithm: create multiple  generations
 	for (int gen = 0; gen < numberOfGenerations; gen++)
 	{
 		//start timer
 		std::clock_t start; double duration; start = std::clock();
 
 		//create new population based on the current one
 		population = Breeder<PermutationCreature>::generateNextGeneration(population);
 		selectSurvivors(population);
 
 		//get data from this generation
 		getDataFromGeneration(population, configuration);
 
 		//stop timer
 		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
 
 		//print data about this generation
 		std::cout << "Gen:" << gen << "\n\t";
 		//double avgFittness = (double)(currentGenPopulationFitness / populationSize) / (configuration.container_width * configuration.container_height);
 		//double maxFittness = (double)generationMaximumFitness / (configuration.container_width * configuration.container_height);
 		//std::cout << "Average fittness: " << avgFittness << "\tBest fittness: " << maxFittness << "  \tTime passed: " << duration << "\n";
 
 		int maxFitness = 0;
 		int maxI = 0;
 		for (int i = 0; i < population.size(); i++)
 		{
 			if(population[i].getFitness() > maxFitness)
 			{
 				maxFitness = population[i].getFitness();
 				maxI = i;
 			}
 		}
 		PrintSolution(population[maxI]);
 		//double areaCovered = (double)generationMaximumFitness / (configuration.container_width * configuration.container_height);
 		//emit boxesReady(generationBoxes[generationBoxes.size() - 1]);
        emit boxesReady(this ,generationBoxes.size() - 1);
 	}
 	//----------------------------------------------------------------------------------------------------------------------------------
 
 	//---Prints results ------/
 	printFinalDataAndSaveResulsts(population, configuration);
}
//-----------------------------------------------------------------------------------------------
void GAThread::PrintSolution(PermutationCreature& c)
{
    std::cout << "fitness " << c.getFitness() << "\n";
    generationBoxes.emplace_back(c.getBoxPositions());
}
//-----------------------------------------------------------------------------------------------
void GAThread::printFinalDataAndSaveResulsts(std::vector<PermutationCreature>& population, Configuration& configuration)
{
// 	Chromozome greedyChromozome;
// 	std::vector<Item> allItems;
// 	for (int i = 0; i < configuration.numberOfItems; i++)
// 	{
// 		allItems.push_back(configuration.items[i]);
// 	}
// 	std::sort(allItems.begin(), allItems.end(), [](const Item& b, const Item& a)
// 	{
// 		int thisSize = b.height*b.width;
// 		int otherSize = a.height*a.width;
// 		return (thisSize > otherSize);
// 	});
// 
// 
// 	for (int i = 0; i < configuration.numberOfItems; i++)
// 	{
// 		greedyChromozome.push_back(allItems[i].id);
// 	}
// 	std::cout << "------------------------------------------------\n";
// 	double maxOverallFittness = (double)overallMaximumFitness / (configuration.container_width * configuration.container_height);
// 	std::cout << "GA best fittness: " << maxOverallFittness << "\n";
// 	std::cout << "Greedy approach fittness: " << double(Creature(greedyChromozome, &configuration).fitness) / (configuration.container_width * configuration.container_height) << "\n";
// 
// 	double max = 0;
// 	int index = 0;
// 	for (int i = 0; i < populationSize; i++)
// 	{
// 		if (population[i].fitness>max)
// 		{
// 			max = population[i].fitness;
// 			index = i;
// 		}
// 	}
// 	std::cout << "Finished." << '\n';
}
//-----------------------------------------------------------------------------------------------
void GAThread::getDataFromGeneration(std::vector<PermutationCreature>& population, Configuration& configuration)
{
// 	generationMaximumFitness = 0;
// 	currentGenPopulationFitness = 0;
// 	for (Creature& indiviual : population)
// 	{
// 		//get data
// 		currentGenPopulationFitness += indiviual.fitness;
// 		generationMaximumFitness = std::max(generationMaximumFitness, indiviual.fitness);
// 	}
// 	overallMaximumFitness = std::max(generationMaximumFitness, overallMaximumFitness);
}

//Creates an array of random creatures
std::vector<PermutationCreature> GAThread::generateFirstGeneration(Configuration& configuration)
{
	std::vector<PermutationCreature> randomCreatures;
	randomCreatures.reserve(populationSize);
	for (int i = 0; i < populationSize; i++)
	{
        randomCreatures.emplace_back(&configuration);
 	}
	return randomCreatures;
}
//---------------------------------------------------------
void GAThread::selectSurvivors(std::vector<PermutationCreature>& population)
{
	std::sort(population.begin(),population.end(), [](const PermutationCreature& a, const PermutationCreature& b){return (a.getFitness() > b.getFitness());});
	population.erase(population.begin() + populationSize, population.end());
}
