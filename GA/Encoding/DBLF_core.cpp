#include "DBLF_core.h"
#include <algorithm>    //for std::max
#include <string>

//input:  Configuration
//output: A new random encoding
//action: Creates a random encoding based on the configuration
DBLF_core::DBLF_core(Configuration* conf)
	:configuration(conf)
{
	booleanGraphsSpaces = nullptr;
	chromozome.reserve(conf->numberOfItems);
	for (int i = 0; i < conf->numberOfItems; i++) { chromozome.push_back(i); }
	std::random_shuffle(chromozome.begin(), chromozome.end());
	calculateFittness();
}
//------------------------------------------------------------------
//input:  Configuration,  chromozome vector
//output: A new encoding based on the given chromozome
//action: Creates a random encoding based on the configuration
DBLF_core::DBLF_core(Configuration* conf, Chromozome chrom, int numberOfItems)
	:configuration(conf)
{
	this->numberOfItemsToPack = numberOfItems;
	this->chromozome.reserve(numberOfItemsToPack);
	for (int i = 0; i < numberOfItemsToPack; i++) { this->chromozome.push_back(chrom[i]); }
	calculateFittness();
}
//----------------------------------------------------------------------------
void DBLF_core::mutate(float mutationChance)
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
void DBLF_core::crossover(DBLF_core parent2, std::vector<DBLF_core>& population)
{
	//genereate crossing points
	int PMX_StartIndex, PMX_EndIndex;
	initializeCrossOverPoints(PMX_StartIndex, PMX_EndIndex);

	//init children:
	Chromozome child1Chromozome, child2Chromozome;
	child1Chromozome.reserve(parent2.chromozome.size());
	child2Chromozome.reserve(parent2.chromozome.size());

	//Create children chromozomes:
	createTwoChildren(child1Chromozome, child2Chromozome, PMX_StartIndex, PMX_EndIndex,
		this->chromozome, parent2.chromozome);


	population.emplace_back(this->configuration, child1Chromozome, 1);
	population.emplace_back(this->configuration, child2Chromozome, 1);
}
//------------------------------------------------------------------
void DBLF_core::initializeCrossOverPoints(int& startPos, int& endPos)
{
	std::uniform_int_distribution<int> cromozomesIndexes(0, this->configuration->numberOfItems - 1);
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
void DBLF_core::createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
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
int DBLF_core::swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap)
{
	int newValue = hash[valueToSwap];
	// make sure new value not needed to swaped as well
	// if it is keep searching till you reach the final value
	while (hash.count(newValue) != 0)
		newValue = hash[newValue];

	return newValue;
}
//---------------------------------------------
int DBLF_core::calculateFittness()
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
		booleanGraphsSpaces[i] = new bool *[containerDim.h]();

		for (j = 0; j < containerDim.h; j++)
		{
			booleanGraphsSpaces[i][j] = new bool[containerDim.w]();
		}
	}


	for (int i = 0; i < containerDim.d; i++)
	{
		for (int j = 0; j < containerDim.h; j++)
		{
			for (int k = 0; k < containerDim.w; k++)
			{
				booleanGraphsSpaces[i][j][k] = false;
			}
		}
	}

	//continue placing items untill its not possible
	for (int i = 0; i < numberOfItemsToPack; i++)
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
	//free memorya
	for (int i = 0; i < containerDim.d; i++)
	{
		for (int j = 0; j < containerDim.h; j++)
			delete[] booleanGraphsSpaces[i][j];
		delete[] booleanGraphsSpaces[i];
	}
	delete[] booleanGraphsSpaces;
	booleanGraphsSpaces = nullptr;

	return fitness;
}
BoxInfo DBLF_core::bottomLeftFill(Item item)
{
	BoxInfo placedPosition(QPoint3D(0, 0, 0), RGB(item.color.r / 256.0f, item.color.g / 256.0f, item.color.b / 256.0f),
		item.dim.w, item.dim.h, item.dim.d, item.value, item.id);
	Dimensions containerDim = configuration->dim;

	int max_z = containerDim.d - item.dim.d;//max possible value of z
	int max_y = containerDim.h - item.dim.h;//max possible value of y
	int max_x = containerDim.w - item.dim.w;//max possible value of x


	for (int _z = 0; _z <= max_z; _z++)//deepest
	{
		for (int _y = 0; _y <= max_y; _y++)//bottomest
		{
			for (int _x = 0; _x <= max_x; _x++) //leftest
			{
				if (isIndexFit(_x, _y, _z, item))
				{

					placedPosition.startingPoint = QPoint3D(_x, _y, _z);
					return placedPosition;
				}
			}
		}
	}


	throw CantFitException();
}
//--------------------------------------------------------------------------------
bool DBLF_core::isIndexFit(int x, int y, int z, Item item)
{
	Dimensions ItemDim = item.dim;
	int max_x = x + ItemDim.w;
	int max_y = y + ItemDim.h;
	int max_z = z + ItemDim.d;

	for (int _z = z; _z < max_z; _z++)
	{
		for (int _y = y; _y < max_y; _y++)
		{
			for (int _x = x; _x < max_x; _x++)
			{
				if (booleanGraphsSpaces[_z][_y][_x])
				{
					return false;
				}
			}
		}
	}

	//if we reach here then it does fit
	for (int _z = z; _z < max_z; _z++)
	{
		for (int _y = y; _y < max_y; _y++)
		{
			for (int _x = x; _x < max_x; _x++)
			{
				booleanGraphsSpaces[_z][_y][_x] = true;
			}
		}
	}
	return true;
}
std::vector<BoxInfo> DBLF_core::getBoxPositions()
{
	return this->boxesPositions;
}


Configuration* DBLF_core::getConfiguration() const
{
	return this->configuration;
}

//-----------------------------------------------------------------------------------------------
// Name : setFitness
// sets the value of the fitness
//-----------------------------------------------------------------------------------------------
void DBLF_core::setFitness(int newFitness)
{
	this->fitness = newFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : getFitness
// Action: return the fitness of this encdoing
//-----------------------------------------------------------------------------------------------
int DBLF_core::getFitness() const
{
	return this->fitness;
}