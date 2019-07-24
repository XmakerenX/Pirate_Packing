#include "GA_Core.h"
#include <sstream>
#include <fstream>
#include <qdir.h>

//--------------------------------------------------------------------------------------------------
// Name:   GA_Core(constructor)
// Input : randomEngine - the random number generator to use for random numbers
// Output: a constructed GA_Core
// Action: Creates a GA_Core
//--------------------------------------------------------------------------------------------------
template <class Creature>
GA_Core<Creature>::GA_Core(Random& _randomEngine)
    :randomEngine(_randomEngine)
{
}

//--------------------------------------------------------------------------------------------------
// Name:   initGeneticAlgorithm
// Input : configuration - the probelm configuration
//         settings - the GA algorithm settings
// Output: none
// Action: clear class data and creates first Generation
//--------------------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::initGeneticAlgorithm(Configuration& configuration, const GA_Settings& settings)
{
	overallMaximumFitness = 0;
	generationMaximumFitness = 0;
	//----Genetic algorithm: first generation ------//
	gen = 0;
	generationData.clear();
	population.clear();
	population = generateFirstGeneration(configuration, settings);
}

//--------------------------------------------------------------------------------------------------
// Name:   getPopulation
//--------------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> GA_Core<Creature>:: getPopulation()
{
	return population;
}

//--------------------------------------------------------------------------------------------------
// Name:   saveGenerationData
// Input : methodPrefix - string repesting the used method for the GA_Core
//         settings - the GA algorithm settings
// Output: the path to the file with the saved generation data
// Action: save the generation data for each generation to file
//--------------------------------------------------------------------------------------------------
template <class Creature>
std::string GA_Core<Creature>::saveGenerationData(const std::string& methodPrefix, const GA_Settings& settings)
{
	if(!QDir("Output").exists())
		if(!QDir().mkdir(("Output")))
			return "";
    
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        
	std::string timeAndDate(30, '\0');
	std::strftime(&timeAndDate[0], timeAndDate.size(), "%Y-%m-%d_%H-%M-%S.dat", std::localtime(&now));
	std::string mutationRateStr = std::to_string(settings.mutationRate);
	mutationRateStr = mutationRateStr.substr(0,4);
	std::string fileName = "Output/" + methodPrefix +  "_" +
	std::to_string(settings.populationSize) + "_" +
	std::to_string(settings.numberOfGenerations) + "_" + 
	mutationRateStr + "_" +
	std::to_string(settings.elitismSizeGroup);
        
	if (settings.nitchingEnabled)
		fileName += "_NE_";
	else
		fileName += "_ND_";
        
	fileName += timeAndDate;
    
	std::ofstream file(fileName);   
	file << "#Value\n";
	for (int i = 0; i < generationData.size(); i++)
		file << i << " " << generationData[i].overallValue << "\n";
        
	file << "\n#Averge\n";
	for (int i = 0; i < generationData.size(); i++)
		file << i << " " << generationData[i].avarageFittness << "\n";
	
	file << "\n#finalBoxPosition\n";
        std::vector<BoxInfo>& bestBoxPosition = generationData.back().bestCreatureBoxInfo;
	for (int i = 0; i < bestBoxPosition.size(); i++)
	{
		file << bestBoxPosition[i].startingPoint.x() << " " << 
		bestBoxPosition[i].startingPoint.y() << " " <<
		bestBoxPosition[i].startingPoint.z() << " " <<
		bestBoxPosition[i].boxWidth << " " << 
		bestBoxPosition[i].boxHeight << " " << 
		bestBoxPosition[i].boxLength << " " << 
		bestBoxPosition[i].value << "\n";
	}
	
        
	file.close();
    
	fileName = fileName.substr(0, fileName.find_first_of('\0'));
	return fileName;
}

//--------------------------------------------------------------------------------------------------
// Name:   nextGeneration
// Input : configuration - the probelm configuration
//         settings - the GA algorithm settings
// Output: is the run of the GA algorithm has ended
// Action: create the next generation
//--------------------------------------------------------------------------------------------------
template <class Creature>
bool GA_Core<Creature>::nextGeneration(Configuration& configuration, const GA_Settings& settings)
{
	//----Genetic algorithm: create next generation
	//start timer
	std::clock_t start; double duration; start = std::clock();
	auto timeStart = std::chrono::high_resolution_clock::now();
	
	std::sort(population.begin(), population.end(),
			[](const Creature& a, const Creature& b) {return (a.getFitness() > b.getFitness()); });
        
        std::vector<Creature> eliteGroup;
	int elitismGroupSize = population.size() * (settings.elitismSizeGroup / 100.0f);
	for (int i = 0; i < elitismGroupSize; i++)
		eliteGroup.push_back(population[i]);
        
	//create new population based on the current one
	population = Breeder<Creature>::generateNextGeneration(population, settings, randomEngine);
	selectSurvivors(population, settings);
	
	for (int i = 0; i < eliteGroup.size(); i++)
		population[population.size() - 1 - i] = eliteGroup[i];
                
	//get data from this generation
	getDataFromGeneration(population, configuration, settings);
	//stop timer
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::chrono::duration<float>  timeDuration = std::chrono::high_resolution_clock::now() - timeStart;
    
	//print data about this generation
	std::cout << "Gen:" << gen << "\n\t";
        GenerationData& currentGenerationData = generationData[generationData.size() - 1];
	std::cout << "Average fittness: " << currentGenerationData.avarageFittness
                  << "\tBest fittness: " << currentGenerationData.bestCreatureFittness << "  \tTime passed: " << timeDuration.count() << "\n"
                  << "\toverall Value " << currentGenerationData.bestCreatureValuePercentage << "%"
                  << "  \toverall Volume " << currentGenerationData.bestCreatureVolumeFilled << "%\n";

	gen++;
	return (gen < settings.numberOfGenerations);
}

//--------------------------------------------------------------------------------------------------
// Name:   getDataFromGeneration
// Input : population - the population to extract the data from
//         configuration - the probelm configuration
//         settings - the GA algorithm settings
// Output: is the run of the GA algorithm has ended
// Action: create the next generation
//--------------------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration, const GA_Settings& settings)
{
	int currentGenPopulationFitness = 0;
	int bestCreatureIndex = 0;
	generationMaximumFitness = population[0].getFitness();

	//find best creature
	for (int i = 1; i < population.size(); i++)
	{
		currentGenPopulationFitness += population[i].getFitness();
		if (generationMaximumFitness < population[i].getFitness())
		{
			generationMaximumFitness = population[i].getFitness();
			bestCreatureIndex = i;
		}
	}

	overallMaximumFitness = std::max(generationMaximumFitness, overallMaximumFitness);
        
	if (!population[bestCreatureIndex].validateConstraints())
	{
		// if Constraints are invalid apply the new penalty 
		for (int i = 0; i < population.size(); i++)
		{
			population[i].calculateFittness();
		}
        }
	
	Configuration * conf = population[bestCreatureIndex].getConfiguration();
	float containerVolume = conf->dim.w * conf->dim.h * conf->dim.d;
	
	generationData.emplace_back(population[bestCreatureIndex].getBoxPositions(),
                                    currentGenPopulationFitness / (float)settings.populationSize, // avarageFittness
                                    generationMaximumFitness,
                                    overallMaximumFitness,
                                    containerVolume,
                                    conf->maxiumValue);                                   
}

//--------------------------------------------------------------------------------------------------
// Name:   generateFirstGeneration
// Input : configuration - the probelm configuration
//         settings - the GA algorithm settings
// Output: a vector of random creatures
// Action: create a random vector of creatures that will be the first generation
//--------------------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> GA_Core<Creature>::generateFirstGeneration(Configuration& configuration, const GA_Settings& settings)
{
	std::vector<Creature> randomCreatures;
	randomCreatures.reserve(settings.populationSize);
	for (int i = 0; i < settings.populationSize; i++)
	{
		randomCreatures.emplace_back(&configuration, randomEngine);
	}
	return randomCreatures;
}

//--------------------------------------------------------------------------------------------------
// Name:   selectSurvivors
// Input : population - the current population
//         settings - the GA algorithm settings
// Output: none
// Action: keep only the fittest creatures in the current population
//--------------------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::selectSurvivors(std::vector<Creature>& population, const GA_Settings& settings)
{
	if (settings.nitchingEnabled)
	{
		std::sort(population.begin(), population.end(),
			[](const Creature& a, const Creature& b) {return (a.getSharedFitness() > b.getSharedFitness()); });
	}
	else
	{
		std::sort(population.begin(), population.end(),
			[](const Creature& a, const Creature& b) {return (a.getFitness() > b.getFitness()); });
	}
	
	population.erase(population.begin() + settings.populationSize, population.end());
}

//--------------------------------------------------------------------------------------------------
// Name : getBoxesInfo
//--------------------------------------------------------------------------------------------------
template <class Creature>
std::vector<BoxInfo>& GA_Core<Creature>::getBoxesInfo(int index)
{
	return generationData[index].bestCreatureBoxInfo;
}

//--------------------------------------------------------------------------------------------------
// Name : getGenerationData
//--------------------------------------------------------------------------------------------------
template <class Creature>
const GenerationData& GA_Core<Creature>::getGenerationData(int index)
{
    return generationData[index];
}

//--------------------------------------------------------------------------------------------------
// Name : getGenerationDataIndex
//--------------------------------------------------------------------------------------------------
template <class Creature>
int GA_Core<Creature>::getGenerationDataIndex()
{
    return generationData.size() - 1;
}

//--------------------------------------------------------------------------------------------------
// Force instantiation of BinaryCreature and PermutationCreature
//--------------------------------------------------------------------------------------------------
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class GA_Core<BinaryCreature>;
template class GA_Core<PermutationCreature>;
