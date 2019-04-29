#include "GA_Core.h"
#include <sstream>
#include <fstream>
#include <qdir.h>

//---------------------------------------------------------------------------------------
template <class Creature>
GA_Core<Creature>::GA_Core(Random& _randomEngine)
    :randomEngine(_randomEngine)
{
}
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
template <class Creature>
std::vector<Creature> GA_Core<Creature>:: getPopulation()
{
	return population;
}
//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>:: replacePopulation(std::vector<Creature>& newPopulation)
{
	population.clear();
	for (Creature& creature : newPopulation)
	{
		population.push_back(creature);
	}
}
//---------------------------------------------------------------------------------------
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
	for (int i = 0; i < generationData.size(); i++)
	{
		file << i << " " << generationData[i].overallValue << "\n";
	}
	
	file.close();
    
	fileName = fileName.substr(0, fileName.find_first_of('\0'));
	return fileName;
}
//---------------------------------------------------------------------------------------
template <class Creature>
bool GA_Core<Creature>::nextGeneration(Configuration& configuration, const GA_Settings& settings)
{
	//----Genetic algorithm: create next generation
	//start timer
	std::clock_t start; double duration; start = std::clock();
	auto timeStart = std::chrono::high_resolution_clock::now();
	
	//create new population based on the current one
	population = Breeder<Creature>::generateNextGeneration(population, settings, randomEngine);
	selectSurvivors(population, settings);
	
        //population = generateFirstGeneration(configuration);
        
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

	/*
	if (gen == 100)
	{
		BinaryCreature::applyDBLF = true;
	}
	if (gen == 110)
	{
		BinaryCreature::applyDBLF = false;
	}
	*/
	return (gen < settings.numberOfGenerations);
}
//------------------------------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration, const GA_Settings& settings)
{
	generationMaximumFitness = std::numeric_limits<int>::min();
	int currentGenPopulationFitness = 0;
	Creature& bestCreature = population[0];

	//find best creature
	for (Creature& indiviual : population)
	{
		currentGenPopulationFitness += indiviual.getFitness();
		int individualFittness = indiviual.getFitness();
		if (generationMaximumFitness < individualFittness)
		{
			generationMaximumFitness = individualFittness;
			bestCreature = indiviual;
		}
	}
	overallMaximumFitness = std::max(generationMaximumFitness, overallMaximumFitness);
        
        if (!bestCreature.validateConstraints())
        {
            // if Constraints are invalid apply the new penalty 
            for (int i = 0; i < population.size(); i++)
            {
                population[i].calculateFittness();
            }
        }
	
	Configuration * conf = bestCreature.getConfiguration();
	float containerVolume = conf->dim.w * conf->dim.h * conf->dim.d;
	
	generationData.emplace_back(bestCreature.getBoxPositions(),
                                    currentGenPopulationFitness / (float)settings.populationSize, // avarageFittness
                                    generationMaximumFitness,
                                    overallMaximumFitness,
                                    containerVolume,
                                    conf->maxiumValue);                                   
}
//-----------------------------------------------------------------------------------------	
//Creates an array of random creatures
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
//---------------------------------------------------------
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
//----------------------------------------------------------
template <class Creature>
std::vector<BoxInfo>& GA_Core<Creature>::getBoxesInfo(int index)
{
	return generationData[index].bestCreatureBoxInfo;
}
//----------------------------------------------------------
template <class Creature>
const GenerationData& GA_Core<Creature>::getGenerationData(int index)
{
    return generationData[index];
}
//-----------------------------------------------------------
template <class Creature>
int GA_Core<Creature>::getGenerationDataIndex()
{
    return generationData.size() - 1;
}

// Force instantiation of BinaryCreature and PermutationCreature
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
template class GA_Core<BinaryCreature>;
template class GA_Core<PermutationCreature>;
