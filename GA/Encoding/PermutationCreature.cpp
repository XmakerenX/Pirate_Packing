#include "PermutationCreature.h"
#include <algorithm>    //for std::max
#include <string>

//input:  Configuration
//output: A new random encoding
//action: Creates a random encoding based on the configuration
PermutationCreature::PermutationCreature(Configuration* conf)
    :configuration(conf)
{
    booleanGraphsSpaces = nullptr;
	chromozome.reserve(conf->numberOfItems);
	for (int i = 0; i < conf->numberOfItems; i++) {	chromozome.push_back(i); }
	std::random_shuffle(chromozome.begin(), chromozome.end());
	calculateFittness();
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
//----------------------------------------------------------------------------
void PermutationCreature::mutate(float mutationChance)
{
	std::vector<float> mutateVec = { mutationChance, 1 - mutationChance };
	std::discrete_distribution<int> mutateDist(mutateVec.begin(), mutateVec.end());

	if (mutateDist(Random::default_engine.getGenerator()) == 0)
	{
		std::uniform_int_distribution<int> IndexeshDist(0, this->configuration->numberOfItems - 1);
		int index1, index2;
		//get the first index
		index1 = IndexeshDist(Random::default_engine.getGenerator());

		//get the second unique index
		do { index2 = IndexeshDist(Random::default_engine.getGenerator()); } while (index1 == index2);

		//switch between the indexes
		int temp = this->chromozome[index1];
		this->chromozome[index1] = this->chromozome[index2];
		this->chromozome[index2] = temp;
	}

}
//----------------------------------------------------------------------------
void PermutationCreature::crossover(PermutationCreature parent2, std::vector<PermutationCreature>& population)
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
	std::uniform_int_distribution<int> cromozomesIndexes(0, this->configuration ->numberOfItems - 1);
	int PMX_StartIndex, PMX_EndIndex;
	do {
		//get the first index
		PMX_StartIndex = cromozomesIndexes(Random::default_engine.getGenerator());
		//get the second unique index
		do
		{
			PMX_EndIndex = cromozomesIndexes(Random::default_engine.getGenerator());
		} while (PMX_EndIndex == PMX_StartIndex);
	} while ((PMX_EndIndex - PMX_StartIndex) >= (this->configuration->numberOfItems / 2));//dont allow too big crossing points   
	
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
	boxesPositions.clear();

		//allocation of a 3d boolean array:
		Configuration* conf = this->configuration;
		Dimensions containerDim = conf->dim;
		int i, j, k;
	    booleanGraphsSpaces = new bool **[containerDim.d]();
		for (i = 0; i < containerDim.d; i++)
		{
			booleanGraphsSpaces[i] = new bool *[containerDim.w]();
			for (j = 0; j < containerDim.w; j++)
				booleanGraphsSpaces[i][j] = new bool[containerDim.h]();
		}


		for (int i = 0; i < containerDim.d; i++)
		{
			for (int j = 0; j < containerDim.w; j++)
			{
				for (int k = 0; k < containerDim.h; k++)
				{
					booleanGraphsSpaces[i][j][k] = false;
				}
			}
		}


		//continue placing items untill its not possible
		for (int i = 0; i < configuration->numberOfItems; i++)
		{
			try//try fitting a lego
			{
				int currentItemIndex = chromozome[i];
				Item item = configuration->items[currentItemIndex];
				this->boxesPositions.push_back(bottomLeftFill(item));
				fitness += item.value;
			}
			catch (CantFitException cantfitEx) // break if you can't
			{
				continue;
			}
		}
		//free memory
		for (int i = 0; i < containerDim.d; i++)
		{
			for (int j = 0; j < containerDim.w; j++)
				delete[] booleanGraphsSpaces[i][j];
			delete[] booleanGraphsSpaces[i];
		}
		delete[] booleanGraphsSpaces;
		booleanGraphsSpaces = nullptr;

	return fitness;
}
BoxInfo PermutationCreature::bottomLeftFill(Item item)
{
	BoxInfo placedPosition(QPoint3D(0,0,0), RGB(item.color.r / 256.0f, item.color.g / 256.0f, item.color.b / 256.0f), item.dim.h, item.dim.w, item.dim.d, item.value);
	Dimensions containerDim = configuration->dim;

	int max_d = containerDim.d - item.dim.d;
	int max_h = containerDim.h - item.dim.h;
	int max_w = containerDim.w - item.dim.w;

	//search row by row from the leftest corner to find a match
	for (int i = 0; i < max_d;i++)
	{
		for (int k = 0; k <max_w; k++)
		{	
			for (int j = 0; j <max_h; j++)
			{
				if (isIndexFit(i, j,k, item))
				{
					
					placedPosition.startingPoint = QPoint3D(j, k, i);
					return placedPosition;
				}
			}
		}
	}
	throw CantFitException();
}
//--------------------------------------------------------------------------------
bool PermutationCreature::isIndexFit(int i, int j, int k, Item item)
{
	Dimensions dim = item.dim;
	int max_d = i + dim.d;
	int max_h = j + dim.h;
	int max_w = k + dim.w;

	for (int d = i; d < max_d; d++)
	{
		for (int h = j; h < max_h; h++)
		{
			for (int w = k; w < max_w; w++)
			{
				if (booleanGraphsSpaces[d][h][w]) return false;
			}
		}
	}

	for (int d = i; d < max_d; d++)
	{
		for (int h = j; h < max_h; h++)
		{
			for (int w = k; w < max_w; w++)
			{
				booleanGraphsSpaces[d][h][w] = true;
			}
		}
	}
	return true;
}
std::vector<BoxInfo> PermutationCreature::getBoxPositions()
{
	return this->boxesPositions;
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
    this->fitness = newFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : getFitness
// Action: return the fitness of this encdoing
//-----------------------------------------------------------------------------------------------
int PermutationCreature::getFitness() const
{
    return this->fitness;
}
