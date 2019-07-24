#include "PermutationCreature.h"
#include <algorithm>    //for std::max
#include <string>

//--------------------------------------------------------------------------------------------------
// Name:   PermutationCreature(constructor)
// Input : Configuration, randomEngine
// Output: A new random encoding
// Action: Creates a random encoding based on the configuration
//--------------------------------------------------------------------------------------------------
PermutationCreature::PermutationCreature(Configuration* conf, Random& randomEngine/* = Random::default_engine*/)
	:configuration(conf)
{
	booleanGraphsSpaces = nullptr;
	chromozome.reserve(conf->numberOfItems);
	for (int i = 0; i < conf->numberOfItems; i++) { chromozome.push_back(i); }
	std::random_shuffle(chromozome.begin(), chromozome.end());
	calculateFittness();
}

//--------------------------------------------------------------------------------------------------
// Name:   PermutationCreature(constructor)
// Input:  Configuration,  chromozome vector
// Output: A new encoding based on the given chromozome
// Action: Creates a random encoding based on the configuration
//--------------------------------------------------------------------------------------------------
PermutationCreature::PermutationCreature(Configuration* conf, Chromozome chrom)
	:configuration(conf)
{
	this->chromozome.reserve(conf->numberOfItems);
	for (int i = 0; i < conf->numberOfItems; i++) { this->chromozome.push_back(chrom[i]); }
}

//--------------------------------------------------------------------------------------------------
// Name:   PermutationCreature(copy constructor)
// Input:  PermutationCreature to be copied
// Output: A copy encoding of the given PermutationCreature
// Action: copy constructor for PermutationCreature
//--------------------------------------------------------------------------------------------------
PermutationCreature::PermutationCreature(const PermutationCreature& copy)
	:boxesPositions(copy.boxesPositions), chromozome(copy.chromozome), activeItems(copy.activeItems), configuration(copy.configuration)
{
	booleanGraphsSpaces = nullptr;
	fitness = copy.fitness;
	sharedFitness = copy.sharedFitness;
}

//--------------------------------------------------------------------------------------------------
// Name:   PermutationCreature(move constructor)
// Input:  PermutationCreature to be moved
// Output: A moved encoding of the given PermutationCreature
// Action: move constructor for PermutationCreature
//--------------------------------------------------------------------------------------------------
PermutationCreature::PermutationCreature(PermutationCreature&& move)
	:boxesPositions(std::move(move.boxesPositions)), chromozome(std::move(move.chromozome)), activeItems(std::move(move.activeItems)), configuration(move.configuration)
{
	booleanGraphsSpaces = nullptr;
	fitness = move.fitness;
	sharedFitness = move.sharedFitness;
}

//--------------------------------------------------------------------------------------------------
// Name:   operator= (copy)
// Input:  PermutationCreature to be copied
// Output: A copy encoding of the given PermutationCreature
// Action: copy assignment operator for PermutationCreature
//--------------------------------------------------------------------------------------------------
PermutationCreature& PermutationCreature::operator=(const PermutationCreature& copy)
{
	booleanGraphsSpaces = nullptr;
	chromozome = copy.chromozome;
	configuration = copy.configuration;
    
	fitness = copy.fitness;
	sharedFitness = copy.sharedFitness;
	boxesPositions = copy.boxesPositions;
	activeItems = copy.activeItems;
        
	return *this;
}

//--------------------------------------------------------------------------------------------------
// Name:   operator= (move)
// Input:  PermutationCreature to be moved
// Output: A moved encoding of the given PermutationCreature
// Action: move assignment operator for PermutationCreature
//--------------------------------------------------------------------------------------------------
PermutationCreature& PermutationCreature::operator=(PermutationCreature&& move)
{
	booleanGraphsSpaces = nullptr;
	chromozome = std::move(move.chromozome);
	configuration = move.configuration;
    
	fitness = move.fitness;
	sharedFitness = move.sharedFitness;
	boxesPositions = std::move(move.boxesPositions);
	activeItems = std::move(move.activeItems);
    
	return *this;
}

//--------------------------------------------------------------------------------------------------
// Name:   mutate
// Input:  mutationChance - the percent chance for mutation to occur
//         randomEngine   - the random number generator to use for random numbers
// Output: none
// Action: based on mutationChance mutate the chromozome of PermutationCreature
//--------------------------------------------------------------------------------------------------
void PermutationCreature::mutate(float mutationChance, Random& randomEngine/* = Random::default_engine*/)
{
	std::vector<float> mutateVec = { mutationChance, 1 - mutationChance };
	std::discrete_distribution<int> mutateDist(mutateVec.begin(), mutateVec.end());

	if (mutateDist(randomEngine.getGenerator()) == 0)
	{
		// randomly choose number of swaps which is between 1%-8% of chromozome size
		int minSwaps = std::max<int>(1, this->chromozome.size() *0.01);
		int maxSwaps = std::max<int>(1, this->chromozome.size() *0.08);
		std::uniform_int_distribution<int> swapsDist(minSwaps, maxSwaps);
		int numberOfSwaps = swapsDist(randomEngine.getGenerator());

                // swap indexes around based on numberOfSwaps
		for (int j = 0; j < numberOfSwaps; j++)
		{
			std::uniform_int_distribution<int> IndexeshDist(0, this->configuration->numberOfItems - 1);
			int index1, index2;
			//get the first index
			index1 = IndexeshDist(randomEngine.getGenerator());

			//get the second unique index
			do { index2 = IndexeshDist(randomEngine.getGenerator()); } while (index1 == index2);

			//switch between the indexes
			int temp = this->chromozome[index1];
			this->chromozome[index1] = this->chromozome[index2];
			this->chromozome[index2] = temp;
		}
	}

}
//--------------------------------------------------------------------------------------------------
// Name:   crossover
// Input:  parent2 - the encoding with which to crossover
//         population  - the vector of population to which to add the newly created children
//         randomEngine   - the random number generator to use for random numbers
// Output: none
// Action: create 2 new children from this encding and parent2 encoding using PMX crossover and adds them to population
//--------------------------------------------------------------------------------------------------
void PermutationCreature::crossover(const PermutationCreature& parent2, std::vector<PermutationCreature>& population, Random& randomEngine/* = Random::default_engine*/) const
{
	//genereate crossing points
	int PMX_StartIndex, PMX_EndIndex;
	initializeCrossOverPoints(PMX_StartIndex, PMX_EndIndex, randomEngine);

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
//--------------------------------------------------------------------------------------------------
// Name:   initializeCrossOverPoints
// Input:  none
// Output: startPos - the starting index for the PMX crossover
//         endPos  -  the ending index for the PMX corssover
//         randomEngine   - the random number generator to use for random numbers
// Action: randomly choose what indexes to use for the PMX corssover
//--------------------------------------------------------------------------------------------------
void PermutationCreature::initializeCrossOverPoints(int& startPos, int& endPos, Random& randomEngine) const
{
	std::uniform_int_distribution<int> cromozomesIndexes(0, this->configuration->numberOfItems - 1);
	int PMX_StartIndex, PMX_EndIndex;
	do {
		//get the first index
		PMX_StartIndex = cromozomesIndexes(randomEngine.getGenerator());
		//get the second unique index
		do
		{
			PMX_EndIndex = cromozomesIndexes(randomEngine.getGenerator());
		} while (PMX_EndIndex == PMX_StartIndex);
	} while ((PMX_EndIndex - PMX_StartIndex) >= (this->configuration->numberOfItems / 2));//dont allow too big crossing points   

	startPos = std::min(PMX_StartIndex, PMX_EndIndex);
	endPos = std::max(PMX_StartIndex, PMX_EndIndex);
}
//--------------------------------------------------------------------------------------------------
// Name:   createTwoChildren
// Input:  min - 
//         max - 
//         parent1_chromozome - the first parent chromozome to use for crossover
//         parent2_chromozome - the second parent chromozome to use for crossover
// Output: child1 - the chromozome for the first child
//         child2 - the chromozome for the Second child
//         randomEngine   - the random number generator to use for random numbers
// Action: randomly choose what indexes to use for the PMX corssover
//--------------------------------------------------------------------------------------------------
void PermutationCreature::createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
	const Chromozome& parent1_chromozome, const Chromozome& parent2_chromozome) const
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

//--------------------------------------------------------------------------------------------------
// Name : swapRepetition
// Input:  hash - tells to what value to swap the given value
//         valueToSwap - the value needed to be swapped 
// Output: the value to swap valueToSwap
// Action: returns to what value to swap the given value(valueToSwap)
//--------------------------------------------------------------------------------------------------
int PermutationCreature::swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap) const
{
	int newValue = hash[valueToSwap];
	// make sure new value not needed to swaped as well
	// if it is keep searching till you reach the final value
	while (hash.count(newValue) != 0)
		newValue = hash[newValue];

	return newValue;
}

//--------------------------------------------------------------------------------------------------
//Name: setSharedFitness
//--------------------------------------------------------------------------------------------------
void PermutationCreature::setSharedFitness(int newSharedFitness)
{
	sharedFitness = newSharedFitness;
}

//--------------------------------------------------------------------------------------------------
// Name : getSharedFitness
//--------------------------------------------------------------------------------------------------
int PermutationCreature::getSharedFitness() const
{
	return sharedFitness;
}

//--------------------------------------------------------------------------------------------------
// Name :  hammingDistance
// Input:  other - the PermutationCreature to check the hammingDistance to
//         valueToSwap - the value needed to be swapped 
// Output: how many different active indexes are between this chromozome and other's chromozome
// Action: return how many different active indexes are between this chromozome and other's chromozome
//--------------------------------------------------------------------------------------------------
int PermutationCreature::hammingDistance(PermutationCreature& other)
{
	int hammingDist = 0;
	
	int minItems = std::min(activeItems.size(), other.activeItems.size());
	for (int index = 0; index < minItems; index++)
	{
		if (this->activeItems[index] != other.activeItems[index])
		{
			hammingDist++;
		}
	}
	
	return hammingDist;
}

//--------------------------------------------------------------------------------------------------
// Name :  calculateFittness
// Input:  this chromozome
// Output: the fitness of this creature
// Action: calculates this creature fitness using DBLF heuristics
//--------------------------------------------------------------------------------------------------
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

	activeItems.clear();
	//continue placing items untill its not possible
	for (int i = 0; i < configuration->numberOfItems; i++)
	{
		try//try fitting a lego
		{
			int currentItemIndex = chromozome[i];
			Item item = configuration->items[currentItemIndex];
			this->boxesPositions.push_back(bottomLeftFill(item));
			activeItems.push_back(currentItemIndex);
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
		for (int j = 0; j < containerDim.h; j++)
			delete[] booleanGraphsSpaces[i][j];
		delete[] booleanGraphsSpaces[i];
	}
	delete[] booleanGraphsSpaces;
	booleanGraphsSpaces = nullptr;

	return fitness;
}

//--------------------------------------------------------------------------------------------------
// Name :  bottomLeftFill
// Input:  item - 
// Output: BoxInfo - 
// Action: 
//--------------------------------------------------------------------------------------------------
BoxInfo PermutationCreature::bottomLeftFill(Item item)
{
	BoxInfo placedPosition(QPoint3D(0, 0, 0), RGB(item.color.r / 256.0f, item.color.g / 256.0f, item.color.b / 256.0f),
									item.dim.w, item.dim.h, item.dim.d, item.value,item.id);
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

//--------------------------------------------------------------------------------------------------
// Name :  isIndexFit
// Input:  x -
//         y -
//         z - 
//         item - 
// Output:  
// Action: 
//--------------------------------------------------------------------------------------------------
bool PermutationCreature::isIndexFit(int x, int y, int z, Item item)
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

//--------------------------------------------------------------------------------------------------
// Name : getBoxPositions
//--------------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------
void PermutationCreature::setFitness(int newFitness)
{
	this->fitness = newFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : getFitness
//-----------------------------------------------------------------------------------------------
int PermutationCreature::getFitness() const
{
	return this->fitness;
}

//-----------------------------------------------------------------------------------------------
// Name : getMinDist
// Action: return the minDist for shared fitness based on the configuration
//-----------------------------------------------------------------------------------------------
unsigned int PermutationCreature::getMinDist()
{
    return configuration->items.size() * 0.15f;
}
