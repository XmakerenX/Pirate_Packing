#include "PermutationCreature.h"
#include <algorithm>    //for std::max
#include <string>

//input:  Configuration
//output: A new random encoding
//action: Creates a random encoding based on the configuration
PermutationCreature::PermutationCreature(Configuration* conf)
    :configuration(conf)
{
	chromozome.reserve(conf->numberOfItems);
	for (int i = 0; i < conf->numberOfItems; i++) {	chromozome.push_back(i); }
	std::random_shuffle(chromozome.begin(), chromozome.end());
}
//------------------------------------------------------------------
//input:  Configuration,  chromozome vector
//output: A new encoding based on the given chromozome
//action: Creates a random encoding based on the configuration
PermutationCreature::PermutationCreature(Configuration* conf, Chromozome chrom)
	:configuration(conf)
{
	this->chromozome.reserve(conf->numberOfItems);
	for (int i = 0; i < conf->numberOfItems; i++) {this->chromozome.push_back(chrom[i]);}
}
void PermutationCreature::mutate(float mutationChance)
{
    
}

void PermutationCreature::crossover(PermutationCreature parent2, std::vector<PermutationCreature> population)
{
	//genereate crossing points
	int PMX_StartIndex ,  PMX_EndIndex;
	initializeCrossOverPoints(PMX_StartIndex, PMX_EndIndex);

	//init children:
	Chromozome child1Chromozome, child2Chromozome;
	child1Chromozome.reserve(parent2.chromozome.size());
	child2Chromozome.reserve(parent2.chromozome.size());

	//Create children chromozomes:
	createTwoChildren(child1Chromozome, child2Chromozome, PMX_StartIndex, PMX_EndIndex,
						this->chromozome, parent2.chromozome);

    
    population.emplace_back(this->configuration, child1Chromozome);
    population.emplace_back(this->configuration, child2Chromozome);
}
//------------------------------------------------------------------
void PermutationCreature::initializeCrossOverPoints(int& startPos, int& endPos)
{
	Configuration* conf = this->configuration;
	std::uniform_int_distribution<int> cromozomesIndexes(0, conf->numberOfItems - 1);
	int PMX_StartIndex, PMX_EndIndex;
	do {
		//get the first index
		PMX_StartIndex = cromozomesIndexes(Random::default_engine.getGenerator());
		//get the second unique index
		do
		{
			PMX_EndIndex = cromozomesIndexes(Random::default_engine.getGenerator());
		} while (PMX_EndIndex == PMX_StartIndex);
	} while ((PMX_EndIndex - PMX_StartIndex) >= (conf->numberOfItems / 2));//dont allow too big crossing points   
	
	startPos = std::min(PMX_StartIndex, PMX_EndIndex);
	endPos = std::max(PMX_StartIndex, PMX_EndIndex);
}
//--------------------------------------------------------------------------------------------------
void PermutationCreature::createTwoChildren(Chromozome& child1, Chromozome& child2,int min,int max,
					   Chromozome parent1_chromozome, Chromozome parent2_chromozome)
{

	std::unordered_map<int, int> child1Hash, child2Hash;

	for (int i = min; i < max; i++)
	{
		if (parent1_chromozome[i] == parent2_chromozome[i])
			continue;
		child1Hash[parent2_chromozome[i]] = parent1_chromozome[i];
		child2Hash[parent1_chromozome[i]] = parent2_chromozome[i];
	}

	for (int i = 0; i < min; i++)
	{
		if (child1Hash.count(parent1_chromozome[i]) == 0)
			child1.push_back(parent1_chromozome[i]);
		else
			child1.push_back(swapRepetition(child1Hash, parent1_chromozome[i]));

		if (child2Hash.count(parent2_chromozome[i]) == 0)
			child2.push_back(parent2_chromozome[i]);
		else
			child2.push_back(swapRepetition(child2Hash, parent2_chromozome[i]));
	}

	for (int i = min; i < max; i++)
	{
		child1.push_back(parent2_chromozome[i]);
		child2.push_back(parent1_chromozome[i]);
	}

	for (int i = max; i < parent1_chromozome.size(); i++)
	{
		if (child1Hash.count(parent1_chromozome[i]) == 0)
			child1.push_back(parent1_chromozome[i]);
		else
			child1.push_back(swapRepetition(child1Hash, parent1_chromozome[i]));

		if (child2Hash.count(parent2_chromozome[i]) == 0)
			child2.push_back(parent2_chromozome[i]);
		else
			child2.push_back(swapRepetition(child2Hash, parent2_chromozome[i]));
	}
}
//---------------------------------------------
int PermutationCreature::swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap)
{
	int newValue = hash[valueToSwap];
	// make sure new value not needed to swaped as well
	// if it is keep searching till you reach the final value
	while (hash.count(newValue) != 0)
		newValue = hash[newValue];

	return newValue;
}
//---------------------------------------------
int PermutationCreature::calculateFittness()
{
    fitness = 0;
    return fitness;
}

std::vector<BoxInfo> PermutationCreature::getBoxPositions()
{
    return std::vector<BoxInfo>();
}


Configuration* PermutationCreature::getConfiguration() const
{
    return this->configuration;
}

//-----------------------------------------------------------------------------------------------
// Name : setFitness
// sets the value of the fitness
//-----------------------------------------------------------------------------------------------
void PermutationCreature::setFitness(int newFitness)
{
    fitness = newFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : getFitness
// Action: return the fitness of this encdoing
//-----------------------------------------------------------------------------------------------
int PermutationCreature::getFitness() const
{
    return fitness;
}
