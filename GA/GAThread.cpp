#include "GAThread.h"
#include "Configuration.h"
#include "Creature.h"
#include "Breeder.h"
#include "GA_Random.h"
#include <vector>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string>
#include "Encoding/BinaryEncoding.h"
#include "Encoding/PermutationEncoding.h"
#include "CreatureNew.h"

//------------------------------------------------------------------------------------------
GAThread::GAThread(int containerWidth, int containerHeight, int nItems, int _guiWidth)
	:overallMaximumFitness(0), generationMaximumFitness(0), configuration(containerWidth, containerHeight, nItems)
{
	guiWidth = _guiWidth;
}

//-----------------------------------------------------------------------------------------------
// run
//-----------------------------------------------------------------------------------------------
void GAThread::run()
{    
	std::cout << "seed " << Random::default_engine.getSeed() << "\n";
	configuration.Reset();
    
    CreatureNew<BinaryEncoding> c1(&configuration);
    CreatureNew<PermutationEncoding> c2(&configuration);
    
	//Configuration configuration(300, 400, 100); 	//----Init a configuration ---//
        
	int curX = 0;
	int curY = 0;
	int maxY = 0;
	itemsRects.clear();
	generationsRects.clear();
	std::cout << "item size :" << configuration.items.size() << "\n";
	std::vector<Item> tempItems = configuration.items;
	std::sort(tempItems.begin(), tempItems.end(), [](const Item& a, const Item& b) {return a.height > b.height; });
	for (int i = 0; i < tempItems.size(); i++)
	{
		if (curX + tempItems[i].width > guiWidth)
		{
			curY += maxY;
			curX = 0;
			maxY = 0;
		}

		itemsRects.emplace_back(curX, curY, curX + tempItems[i].width, curY + tempItems[i].height, tempItems[i].color);
		curX += tempItems[i].width;
		if (tempItems[i].height > maxY)
			maxY = tempItems[i].height;
	}
	emit itemRectsUpdate(curY + maxY);
	
	emit GAStarted();
	HybridGeneticAlgorithm(configuration);     //apply genetic algorithm on this configuration
	emit GAFinished();
}

//-----------------------------------------------------------------------
void GAThread::HybridGeneticAlgorithm(Configuration& configuration)
{
	overallMaximumFitness = 0;
	//----Genetic algorithm: first generation ------//
	std::vector<Creature> population = generateFirstGeneration(configuration);
	
	//----Genetic algorithm: create multiple  generations
	for (int gen = 0; gen < numberOfGenerations; gen++)
	{
		//start timer
		std::clock_t start; double duration; start = std::clock();
	
		//create new population based on the current one
		population = Breeder::generateNextGeneration(population);
		selectSurvivors(population);
	
		//get data from this generation
		getDataFromGeneration(population, configuration);
	
		//stop timer
		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	
		//print data about this generation
		std::cout << "Gen:" << gen << "\n\t";
		double avgFittness = (double)(currentGenPopulationFitness / populationSize) / (configuration.container_width * configuration.container_height);
		double maxFittness = (double)generationMaximumFitness / (configuration.container_width * configuration.container_height);
		std::cout << "Average fittness: " << avgFittness << "\tBest fittness: " << maxFittness << "  \tTime passed: " << duration << "\n";

		int maxFitness = 0;
		int maxI = 0;
		for (int i = 0; i < population.size(); i++)
		{
			if(population[i].fitness > maxFitness)
			{
				maxFitness = population[i].fitness;
				maxI = i;
			}
		}
		PrintSolution(population[maxI]);
		double areaCovered = (double)generationMaximumFitness / (configuration.container_width * configuration.container_height);
		emit rectsReady(generationsRects.size() - 1, generationMaximumFitness, areaCovered, currentGenPopulationFitness);
	}
	//----------------------------------------------------------------------------------------------------------------------------------

	//---Prints results ------/
	printFinalDataAndSaveResulsts(population, configuration);
}
//-----------------------------------------------------------------------------------------------
void GAThread::PrintSolution(Creature& c)
{
	const std::vector<Item> items = c.configuration->items;
	const std::vector<Position> itemPositions =  c.creatureGraphBin.itemsPosition;
	std::vector<Rect> rects;
	
	for (int i = 0 ; i < itemPositions.size(); i++)
	{
		int itemIndex = c.creatureGraphBin.chromozome[i];
		int xPos = itemPositions[i].pos_x;
		int yPos = c.configuration->container_height - itemPositions[i].pos_y;
		rects.emplace_back(xPos , yPos - items[itemIndex].height, xPos + items[itemIndex].width, yPos, items[itemIndex].color);
	}
	
	generationsRects.emplace_back(std::move(rects));
}
//-----------------------------------------------------------------------------------------------
void GAThread::printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration)
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
	double maxOverallFittness = (double)overallMaximumFitness / (configuration.container_width * configuration.container_height);
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
	std::cout << "Finished." << '\n';
}
//-----------------------------------------------------------------------------------------------
void GAThread::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration)
{
	generationMaximumFitness = 0;
	currentGenPopulationFitness = 0;
	for (Creature& indiviual : population)
	{
		//get data
		currentGenPopulationFitness += indiviual.fitness;
		generationMaximumFitness = std::max(generationMaximumFitness, indiviual.fitness);
	}
	overallMaximumFitness = std::max(generationMaximumFitness, overallMaximumFitness);
}

//Creates an array of random creatures
std::vector<Creature> GAThread::generateFirstGeneration(Configuration& configuration)
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
void GAThread::selectSurvivors(std::vector<Creature>& population)
{
	std::sort(population.begin(),population.end(), [](const Creature& a, const Creature& b){return (a.fitness > b.fitness);});
	population.erase(population.begin() + populationSize, population.end());
}
