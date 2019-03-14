// 2D_Bin_Packing_Solver.cpp : Defines the entry point for the console application.
//

#include "includes/stdafx.h"
#include "GA/Configuration.h"
#include "GA/Creature.h"
#include "GA/Breeder.h"
#include <vector>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string> 

const int numberOfGenerations = 120;
const int populationSize = 100;
double avgFittness = 0; double maxFittness = 0; double maxOverallFittness = 0;
std::vector<Creature> generateFirstGeneration(Configuration& configuration);
void printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration);
void getDataFromGeneration(std::vector<Creature>& population, double& avgFittness, double& overallMaximum, double& generationMaximum, Configuration& configuration);
void selectSurvivors(std::vector<Creature>& population);
void HybridGeneticAlgorithm(Configuration& configuration);
//################################################################################################################

int main()
{
	Configuration configuration(300, 400, 60); 	//----Init a configuration ---//
	HybridGeneticAlgorithm(configuration);     //apply genetic algorithm on this configuration
	return 0;
}
//-----------------------------------------------------------------------
void HybridGeneticAlgorithm(Configuration& configuration)
{
	//----Genetic algorithm: first generation ------//
	std::vector<Creature> population = generateFirstGeneration(configuration);

	////----Genetic algorithm: create multiple  generations
	for (int gen = 0; gen < numberOfGenerations; gen++)
	{

		//start timer
		std::clock_t start; double duration; start = std::clock();

		//create new population based on the current one
		population = Breeder::generateNextGeneration(population);// TODO: verify there is no copy happening here!            
		selectSurvivors(population);

		//get data from this generation/
		maxFittness = 0;  avgFittness = 0; // init data about generation
		getDataFromGeneration(population, avgFittness, maxOverallFittness, maxFittness, configuration);

		//stop timer
		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

		//print data about this generation
		std::cout << "Gen:" << gen << "\n\t";
		std::cout << "Average fittness: " << avgFittness << "\tBest fittness: " << maxFittness << "  \tTime passed: " << duration << "\n";
	}
	//----------------------------------------------------------------------------------------------------------------------------------

	//---Prints results ------/
	printFinalDataAndSaveResulsts(population, configuration);

}
//-----------------------------------------------------------------------------------------------
void printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration)
{
	Chromozome greedyChromozome;
	std::vector<Item> allItems;
	for (int i = 0; i < configuration.numberOfItems; i++)
	{
		allItems.push_back(configuration.items[i]);
	}
	std::sort(allItems.begin(), allItems.end(), [](const Item& b, const Item& a)
	{
		int thisSize = b.height*b.width;
		int otherSize = a.height*a.width;
		return (thisSize > otherSize);
	});


	for (int i = 0; i < configuration.numberOfItems; i++)
	{
		greedyChromozome.push_back(allItems[i].id);
	}

	std::cout << "------------------------------------------------\n";
	std::cout << "GA best fittness: " << maxOverallFittness << "\n";
	std::cout << "Greedy approach fittness: " << double(Creature(greedyChromozome, &configuration).fitness) / (configuration.container_width * configuration.container_height) << "\n";

	double max = 0;
	int index = 0;
	for (int i = 0; i < populationSize; i++)
	{
		if (population[i].fitness>max)
		{
			max = population[i].fitness;
			index = i;
		}
	}
	std::cout << "Saving best GA creature as .BMP files..." << '\n';
	//BmpCreatreSaver bmpSaverForGA(population[index].creatureGraphBin, "GABest.bmp");
	std::cout << "Saving greedy creature as .BMP files..." << '\n';
	//BmpCreatreSaver bmpSaverforGreedy(Creature(greedyChromozome, &configuration).creatureGraphBin, "greedy.bmp");
	std::cout << "Finished." << '\n';
}
//-----------------------------------------------------------------------------------------------
void getDataFromGeneration(std::vector<Creature>& population, double& avgFittness,double& overallMaximum,double& generationMaximum,Configuration& configuration)
{
	for (Creature& indiviual : population)
	{
		//get data
		double currentFittness = (double)indiviual.fitness / (configuration.container_width * configuration.container_height);
		avgFittness += currentFittness;
		generationMaximum = std::max(generationMaximum, currentFittness);
		if (overallMaximum < generationMaximum)
		{
			overallMaximum = generationMaximum;
		}
	}
	avgFittness = avgFittness / populationSize;
}
//Creates an array of random creatures
std::vector<Creature> generateFirstGeneration(Configuration& configuration)
{
	std::vector<Creature> randomCreatures;
    randomCreatures.reserve(populationSize);
	for (int i = 0; i < populationSize; i++)
	{
        randomCreatures.emplace_back(Creature::createRandomChromozome(configuration), &configuration);
	}
	return randomCreatures;
}
//---------------------------------------------------------

void selectSurvivors(std::vector<Creature>& population)
{
    std::sort(population.begin(),population.end(), [](const Creature& a, const Creature& b){return (a.fitness > b.fitness);});
    population.erase(population.begin() + populationSize, population.end());
}
 
