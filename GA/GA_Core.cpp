#include "GA_Core.h"
#include "GA_Settings.h"




template <class Creature>
Configuration GA_Core<Creature>::configuration;
template <class Creature>
int GA_Core<Creature>::overallMaximumFitness;
template <class Creature>
int GA_Core<Creature>::generationMaximumFitness;
template <class Creature>
int GA_Core<Creature>::currentGenPopulationFitness;
template <class Creature>
std::vector<std::vector<BoxInfo>> GA_Core<Creature>::generationBoxes;



template <class Creature>
void GA_Core<Creature>::HybridGeneticAlgorithm(Configuration& configuration, GAThread* callingThread)
{
	overallMaximumFitness = 0;
	//----Genetic algorithm: first generation ------//
	std::vector<Creature> population = generateFirstGeneration(configuration);

	//----Genetic algorithm: create multiple  generations
	for (unsigned int gen = 0; gen < GA_Settings::numberOfGenerations; gen++)
	{
		//start timer
		std::clock_t start; double duration; start = std::clock();

		//create new population based on the current one
		population = Breeder<Creature>::generateNextGeneration(population);
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
			if (population[i].getFitness() > maxFitness)
			{
				maxFitness = population[i].getFitness();
				maxI = i;
			}
		}
		PrintSolution(population[maxI]);
		//double areaCovered = (double)generationMaximumFitness / (configuration.container_width * configuration.container_height);
		//emit boxesReady(generationBoxes[generationBoxes.size() - 1]);
		//emit boxesReady(callingThread, generationBoxes.size() - 1);
		callingThread->emitBoxReady(generationBoxes.size()-1);
		//----------------------------------------------------------------------------------------------------------------------------------
	}
	//---Prints results ------/
	printFinalDataAndSaveResulsts(population, configuration);
}
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::PrintSolution(Creature& c)
{

}
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration)
{
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
}
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration)
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
//-----------------------------------------------------------------------------------------	
//Creates an array of random creatures
template <class Creature>
std::vector<Creature> GA_Core<Creature>::generateFirstGeneration(Configuration& configuration)
{
	std::vector<Creature> randomCreatures;
	randomCreatures.reserve(GA_Settings::populationSize);
	for (int i = 0; i < GA_Settings::populationSize; i++)
	{
		randomCreatures.emplace_back(&configuration);
	}
	return randomCreatures;
}
//---------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::selectSurvivors(std::vector<Creature>& population)
{
	std::sort(population.begin(), population.end(), [](const Creature& a, const Creature& b) {return (a.getFitness() > b.getFitness()); });
	population.erase(population.begin() + GA_Settings::populationSize, population.end());
}
//----------------------------------------------------------
template <class Creature>
std::vector<std::vector<BoxInfo>>& GA_Core<Creature>::getBoxesInfo()
{
	return generationBoxes;
};
//-----------------------------------------------------------


// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class GA_Core<BinaryCreature>;
template class GA_Core<PermutationCreature>;