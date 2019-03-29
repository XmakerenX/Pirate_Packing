#include "GA_Core.h"
#include "GA_Settings.h"

//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::initGeneticAlgorithm(Configuration& configuration)
{
	overallMaximumFitness = 0;
	generationMaximumFitness = 0;
	currentGenPopulationFitness = 0;
	//----Genetic algorithm: first generation ------//
	gen = 0;
        generationBoxes.clear();
	population.clear();
	population = generateFirstGeneration(configuration);
}
//---------------------------------------------------------------------------------------
template <class Creature>
bool GA_Core<Creature>::nextGeneration(Configuration& configuration)
{
	//----Genetic algorithm: create next generation
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
	int avgFittness = currentGenPopulationFitness / GA_Settings::populationSize;
	int maxFittness = generationMaximumFitness;
	std::cout << "Average fittness: " << avgFittness << "\tBest fittness: " << maxFittness << "  \tTime passed: " << duration << "\n";
	
	int maxFitness = std::numeric_limits<int>::min();
	int maxI = 0;
	for (int i = 0; i < population.size(); i++)
	{
		if (population[i].getFitness() > maxFitness)
		{
			maxFitness = population[i].getFitness();
			maxI = i;
		}
	}
	
	if (!population[maxI].validateConstraints())
	{
		for (int i = 0; i < population.size(); i++)
		{
			population[i].calculateFittness();
		}
	}
	
	PrintSolution(population[maxI]);
	gen++;
	
	if (gen < GA_Settings::numberOfGenerations)
	{
		return true;
	}
	else
	{
		//---Prints results ------/
		printFinalDataAndSaveResulsts(population, configuration);
		return false;
	}
}
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::PrintSolution(Creature& c)
{
    generationBoxes.emplace_back(c.getBoxPositions());
    // print more statasitcs
    Configuration * conf = c.getConfiguration();
    int overallValue = 0;
    int overallVolume = 0;
    std::vector<BoxInfo>& boxesPositions = generationBoxes[generationBoxes.size() - 1];
    for (BoxInfo& boxinfo : boxesPositions)
    {
        overallValue += boxinfo.value;
        overallVolume += boxinfo.boxWidth * boxinfo.boxHeight * boxinfo.boxLength;
    }
    float containerVolume = conf->dim.w * conf->dim.h * conf->dim.d;
    std::cout << "\toverall Value " << overallValue << " " << overallValue / (float)conf->maxiumValue << 
                 "%\toverall Volume " << overallVolume << "  " << overallVolume /  (containerVolume)  << "%\n"; 
}
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration)
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
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration)
{
	generationMaximumFitness = std::numeric_limits<int>::min();
	currentGenPopulationFitness = 0;
	for (Creature& indiviual : population)
	{
		//get data
		currentGenPopulationFitness += indiviual.getFitness();
		generationMaximumFitness = std::max(generationMaximumFitness, indiviual.getFitness());
	}
	overallMaximumFitness = std::max(generationMaximumFitness, overallMaximumFitness);
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
template <class Creature>
int GA_Core<Creature>::getBoxesInfoIndex()
{
    std::cout << generationBoxes.size() - 1 << "\n";
    return generationBoxes.size() - 1;
}

// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class GA_Core<BinaryCreature>;
template class GA_Core<PermutationCreature>;
