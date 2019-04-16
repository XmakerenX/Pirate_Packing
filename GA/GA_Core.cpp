#include "GA_Core.h"
#include "GA_Settings.h"
#include <fstream>
#include <qdir.h>

//---------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::initGeneticAlgorithm(Configuration& configuration)
{
	overallMaximumFitness = 0;
	generationMaximumFitness = 0;
	//----Genetic algorithm: first generation ------//
	gen = 0;
        generationData.clear();
	population.clear();
	population = generateFirstGeneration(configuration);
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
void GA_Core<Creature>::saveGenerationData(const std::string& methodPrefix)
{
	if(!QDir("Output").exists())
		if(!QDir().mkdir(("Output")))
			return;
    
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        
	std::string timeAndDate(30, '\0');
	std::strftime(&timeAndDate[0], timeAndDate.size(), "%Y-%m-%d_%H-%M-%S.dat", std::localtime(&now));
	std::string fileName = "Output/" + methodPrefix + "_" + timeAndDate;
    
	std::ofstream file(fileName);   
	for (int i = 0; i < generationData.size(); i++)
	{
		file << i << " " << generationData[i].overallValue << "\n";
	}
	
	file.close();
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
	
        //population = generateFirstGeneration(configuration);
        
	//get data from this generation
	getDataFromGeneration(population, configuration);
	//stop timer
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
    
	//print data about this generation
	std::cout << "Gen:" << gen << "\n\t";
        GenerationData& currentGenerationData = generationData[generationData.size() - 1];
	std::cout << "Average fittness: " << currentGenerationData.avarageFittness
                  << "\tBest fittness: " << currentGenerationData.bestCreatureFittness << "  \tTime passed: " << duration << "\n"
                  << "\toverall Value " << currentGenerationData.bestCreatureValuePercentage << "%"
                  << "  \toverall Volume " << currentGenerationData.bestCreatureVolumeFilled << "%\n";

	gen++;
	return (gen < GA_Settings::numberOfGenerations);
}
//------------------------------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration)
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
                                    currentGenPopulationFitness / (float)GA_Settings::populationSize, // avarageFittness
                                    generationMaximumFitness,
                                    overallMaximumFitness,
                                    containerVolume,
                                    conf->maxiumValue);                                   
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

	std::sort(population.begin(), population.end(), [](const Creature& a, const Creature& b) {return (a.getSharedFitness() > b.getSharedFitness()); });
	population.erase(population.begin() + GA_Settings::populationSize, population.end());
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
