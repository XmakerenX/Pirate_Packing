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
	std::cout << "Average fittness: " << currentGenerationData.avarageFittness
		<< "\tBest fittness: " << currentGenerationData.bestCreature_Fittness << "  \tTime passed: " << duration << "\n"
		<< "\toverall Value " << currentGenerationData.bestCreature_ValuePercentage << "%"
		<< "  \toverall Volume " << currentGenerationData.bestCreature_VolumeFilled << "%\n";

	gen++;
	return (gen < GA_Settings::numberOfGenerations);
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
//------------------------------------------------------------------------------------------------------------
template <class Creature>
void GA_Core<Creature>::getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration)
{
	generationMaximumFitness = 0;
	currentGenPopulationFitness = 0;
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
	currentGenerationData.avarageFittness = currentGenPopulationFitness / GA_Settings::populationSize;
	currentGenerationData.bestCreature_Fittness = generationMaximumFitness;

	overallMaximumFitness = std::max(generationMaximumFitness, overallMaximumFitness);
	
	generationBoxes.emplace_back(bestCreature.getBoxPositions());
	Configuration * conf = bestCreature.getConfiguration();
	int overallValue = 0;
	int overallVolume = 0;
	std::vector<BoxInfo>& boxesPositions = generationBoxes[generationBoxes.size() - 1];
	for (BoxInfo& boxinfo : boxesPositions)
	{
		overallValue += boxinfo.value;
		overallVolume += boxinfo.boxWidth * boxinfo.boxHeight * boxinfo.boxLength;
	}
	float containerVolume = conf->dim.w * conf->dim.h * conf->dim.d;
	
	currentGenerationData.bestCreature_ValuePercentage = overallValue / (float)conf->maxiumValue;
	currentGenerationData.bestCreature_VolumeFilled = overallVolume / (containerVolume);
	
	bestCreatureBoxInfo.clear();
	bestCreatureBoxInfo.reserve(boxesPositions.size());
	for (int i = 0; i < boxesPositions.size();i++)
	{
		bestCreatureBoxInfo.emplace_back(boxesPositions[i]);
	}
	currentGenerationData.bestCreature_BoxInfo = bestCreatureBoxInfo;
	currentGenerationData.bestFittnessUntillThisGeneration = overallMaximumFitness;
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
