#include "BinaryCreature.h"
#include "DBLF_core.h"
int BinaryCreature::penaltyWeight = 1;
bool BinaryCreature::applyDBLF = false;


//Binary Creature:

//The binary creature was created in order to solve the pirate packing problem in a more efficient (and perhaps even better) manner
//than the solution provided by the permutation creature, which, while possibly good, still lacks the much needed property of efficiency 
//and may not reach the best solution after all.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Chromozome~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//The chromozome of the binary creature consists of N(N = total number of items) genes in a row,
// where the i-th gene represent the i-th item.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Gene~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//A gene is composed of the following bits:
//[itemState] [orientation]   [pos_x]     [pos_y]     [pos z]
//   1 bit       3 bit        <x>bits    <y> bits      <z> bits

// Item state = a boolean to determine whether the item is in the container or not. 1 = true , 0 = false
// oriention  = since the items are 3D, each one of them has 6 possible orientions.
// pos_x      = x position of the item inside the container , number of bits depend on the max dimension of the container   
// pos_y      = y position of the item inside the container , number of bits depend on the max dimension of the container  
// pos_z      = z position of the item inside the container , number of bits depend on the max dimension of the container 

//-----------------------------------------------------------------------------------------------
// Name : BinaryCreature
// Input: Configuration of the problem 
// Output: A random Binary encdoing for a solution for the problem
// Action: Creates a random Binary encdoing for a solution for the given problem
//-----------------------------------------------------------------------------------------------
BinaryCreature::BinaryCreature(Configuration* conf, Random& randomEngine/* = Random::default_engine*/)
    :configuration(conf)
{
    chromozome = generateChromosome(configuration->totalBitsNum, randomEngine); //create the creature data
    // repair collisions and overflowing values(e.g: orientation is only allowed
    // to range from 0 to 5 but could become more due to the nature of the crossover
    // and mutation, so it needs to be fixed)
    repairChromosome(randomEngine);
    calculateFittness();
}

//-----------------------------------------------------------------------------------------------
// Name : BinaryCreature
// Input: Configuration of the problem and the encdoing chromozome
// Output: Binary encdoing built with the given chromozome
// Action:  Creates a Binary encdoing with the given chromozome
// * copies the _chromozome
//-----------------------------------------------------------------------------------------------
BinaryCreature::BinaryCreature(Configuration* config, const DynamicBitSet& _chromozome)
    :chromozome(_chromozome), configuration(config)
{
}

//-----------------------------------------------------------------------------------------------
// Name : BinaryCreature
// Input: Configuration of the problem and the encdoing chromozome
// Output: Binary encdoing built with the given chromozome
// Action:  Creates a Binary encdoing with the given chromozome
// * moves the _chromozome
//-----------------------------------------------------------------------------------------------
BinaryCreature::BinaryCreature(Configuration* config, DynamicBitSet&& _chromozome)
    :chromozome(std::move(_chromozome)), configuration(config)
{
}

//-----------------------------------------------------------------------------------------------
// Name : BinaryCreature
// Input: BinaryCreature to copy
// Output: Binary encdoing copy of the given BinaryCreature 
// Action: copy constructor for BinaryCreature
// * copies copy.chromozome
//-----------------------------------------------------------------------------------------------
BinaryCreature::BinaryCreature(const BinaryCreature& copy)
    :boxesPositions(copy.boxesPositions), chromozome(copy.chromozome), activeItemsMask(copy.activeItemsMask) ,configuration(copy.configuration)
{
    fitness = copy.fitness;
    sharedFitness = copy.sharedFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : BinaryCreature
// Input: BinaryCreature to move
// Output: Binary encdoing with the chromosome moved from the given BinaryCreature
// Action: move constructor for BinaryCreature
// * moves the move.chromozome
//-----------------------------------------------------------------------------------------------
BinaryCreature::BinaryCreature(BinaryCreature&& move)
    :boxesPositions(std::move(move.boxesPositions)), chromozome(std::move(move.chromozome)), activeItemsMask(std::move(move.activeItemsMask)),configuration(move.configuration)
{
    fitness = move.fitness;
    sharedFitness = move.sharedFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : operator=(copy)
// Input: BinaryCreature to copy
// Output: Binary encdoing copy of the given BinaryCreature 
// Action: copy operator= for BinaryCreature
// * copies copy.chromozome
//-----------------------------------------------------------------------------------------------
BinaryCreature& BinaryCreature::operator=(const BinaryCreature& copy)
{
    boxesPositions = copy.boxesPositions;
    chromozome = copy.chromozome;
    activeItemsMask = copy.activeItemsMask;
    configuration = copy.configuration;
    fitness = copy.fitness;
    sharedFitness = copy.sharedFitness;
    
    return *this;
}

//-----------------------------------------------------------------------------------------------
// Name : operator=(move)
// Input: BinaryCreature to move
// Output: Binary encdoing move of the given BinaryCreature 
// Action: move operator= for BinaryCreature
// * moves move.chromozome
//-----------------------------------------------------------------------------------------------
BinaryCreature& BinaryCreature::operator=(BinaryCreature&& move)
{
    boxesPositions = std::move(move.boxesPositions);
    chromozome = std::move(move.chromozome);
    activeItemsMask = std::move(move.activeItemsMask);
    configuration = move.configuration;
    fitness = move.fitness;
    sharedFitness = move.sharedFitness;
    
    return *this;    
}

//-----------------------------------------------------------------------------------------------
// Name : generateChromosome
// Input: totalBitsNum - number of bits for the chromosome
// Output: A random DynamicBitSet of size totalBitsNum
// Action: Creates a random DynamicBitSet with size totalBitsNum
//-----------------------------------------------------------------------------------------------
DynamicBitSet BinaryCreature::generateChromosome(long unsigned int totalBitsNum, Random& randomEngine)
{
    // get max value of long unsigned int
    long unsigned int  maxNum = 0;
    maxNum--;
    // generate values between 0 and the max value for long unsigned int
    std::uniform_int_distribution<long unsigned int> longDistributaion(0, maxNum);
    
    std::vector<long unsigned int> chromosomeInput;
    long unsigned int curTotalBitsNum = totalBitsNum;
    
    // DynamicBitSet bigger than 64bits need to be set by a vector of long unsigned int
    // so randomly generate such a vector with the last element being the reminder bits left
    while (curTotalBitsNum > 0)
    {
        if (curTotalBitsNum > 64)
        {
            chromosomeInput.push_back(longDistributaion(randomEngine.getGenerator()));
            curTotalBitsNum -= 64;
        }
        else
        {
            long unsigned int reminderMaxNum = std::pow(2, curTotalBitsNum);
            reminderMaxNum--;
            std::uniform_int_distribution<long unsigned int> reminderDistributaion(0, reminderMaxNum);
            chromosomeInput.push_back(reminderDistributaion(randomEngine.getGenerator()));
            curTotalBitsNum -= curTotalBitsNum;
        }
    }
    
    DynamicBitSet chromosome = DynamicBitSet(chromosomeInput.begin(), chromosomeInput.end());
    // the chromosome size will be in multiplication of 64 so shirnk it to totalBitsNum
    // to avoid usesless zeros at the binary string start
    chromosome.resize(totalBitsNum);
    
    DynamicBitSet itemMask = DynamicBitSet(chromosome.size(), 0x8000 );
    for (int i = 0; i < configuration->items.size() - 1; i++)
    {
        itemMask = itemMask << 1;
        itemMask[0] = 1;
        itemMask = itemMask << (configuration->bitsPerItem - 1);
    }
    
//     // limit the number of items per chromosome
//     std::uniform_int_distribution<long unsigned int> itemDistributaion(0, configuration->items.size() - 1);
//     for (int i = 0; i < 10; i++)
//     {
//         itemMask[itemDistributaion(randomEngine.getGenerator()) * configuration->bitsPerItem + (3 + 3 * configuration->coordinateBits)] = 0;
//     }
//     
//     itemMask.flip();
//     chromosome = chromosome & itemMask;
//     chromosome = chromosome | itemMask;
    
    return chromosome;
}
//-----------------------------------------------------------------------------------------------
// Name : repairChromosome
// Input: this encdoing chromozome with possible invalid values
// Output: this encdoing chromozome with valid values
// Action: checks for invalid values in chromozome amd fixes them
// * invalid values are X,Y,Z that puts the item outside the container
//-----------------------------------------------------------------------------------------------
void BinaryCreature::repairChromosome(Random& randomEngine)
{
    std::vector<Box> itemBoxes;
    std::vector<long int> valuesOfItems;
    std::vector<long int> boxId;
    std::vector<bool> isItemStillIn;

    DynamicBitSet itemMask = configuration->itemMask;
    for (int i = 0; i < configuration->items.size(); i++)
    {
        ItemInfo itemInfo;
        if (getItemInfo(itemMask, i, itemInfo))
        {
            bool wasRepaired = false;
            if (itemInfo.orientaion > 5)
            {
                std::uniform_int_distribution<unsigned int> orientaionDist(0, 5);
                itemInfo.orientaion = orientaionDist(randomEngine.getGenerator());
                itemInfo.width = configuration->items[i].dim.w;
                itemInfo.height = configuration->items[i].dim.h;
                itemInfo.depth = configuration->items[i].dim.d;
                adjustDimensionsToOrientation(itemInfo.orientaion, itemInfo.width, itemInfo.height, itemInfo.depth);
                wasRepaired = true;
            }

            // repair if needed the x and y coordinates
            if (itemInfo.x + itemInfo.width > configuration->dim.w)
            {
                std::uniform_int_distribution<unsigned int> xDist(0, configuration->dim.w - itemInfo.width);
                itemInfo.x = xDist(randomEngine.getGenerator());
                //itemInfo.x = configuration->dim.w - itemInfo.width;
                wasRepaired = true;
            }

            if (itemInfo.y + itemInfo.height > configuration->dim.h)
            {
                std::uniform_int_distribution<unsigned int> yDist(0, configuration->dim.h - itemInfo.height);
                itemInfo.y = yDist(randomEngine.getGenerator());
                //itemInfo.y = configuration->dim.h - itemInfo.height;
                wasRepaired = true;
            }

            if (itemInfo.z + itemInfo.depth > configuration->dim.d)
            {
                std::uniform_int_distribution<unsigned int> zDist(0, configuration->dim.d - itemInfo.depth);
                itemInfo.z = zDist(randomEngine.getGenerator());
                //itemInfo.z = configuration->dim.d - itemInfo.depth;
                wasRepaired = true;
            }

            if (wasRepaired)
            {
                DynamicBitSet newOrientation(chromozome.size(), itemInfo.orientaion);
                newOrientation = newOrientation << configuration->coordinateBits * 3;
                DynamicBitSet newX(chromozome.size(), itemInfo.x);
                newX = newX << configuration->coordinateBits * 2;
                DynamicBitSet newY(chromozome.size(), itemInfo.y);
                newY = newY << configuration->coordinateBits;
                DynamicBitSet newZ(chromozome.size(), itemInfo.z);
                DynamicBitSet updatedXYValue = newOrientation | newX | newY | newZ;
                // copy the updated value to chromosome
                for (unsigned int j = 0; j < (configuration->bitsPerItem - 1); j++)
                {
                    chromozome[configuration->bitsPerItem * i + j] = updatedXYValue[j];
                }
            }

            itemBoxes.emplace_back(itemInfo.x, itemInfo.y, itemInfo.z,
                                   itemInfo.x + itemInfo.width,
                                   itemInfo.y + itemInfo.height,
                                   itemInfo.z + itemInfo.depth);
            valuesOfItems.push_back(configuration->items[i].value);
            boxId.push_back(i);
            isItemStillIn.push_back(true);
        }
        
        itemMask = itemMask << configuration->bitsPerItem;
    }

    // remove conflicts based on value
    for (int i = 0; i < itemBoxes.size(); i++)
    {
        if (!isItemStillIn[i])
            continue;

        if (itemBoxes[i].getWidth() > configuration->dim.w)
        {
            chromozome[configuration->bitsPerItem * boxId[i] + (3 + 3 * configuration->coordinateBits)] = 0;
            isItemStillIn[i] = false;
            continue;
        }
                
        if (itemBoxes[i].getHeight() > configuration->dim.h)
        {
            chromozome[configuration->bitsPerItem * boxId[i] + (3 + 3 * configuration->coordinateBits)] = 0;
            isItemStillIn[i] = false;
            continue;
        }
        if (itemBoxes[i].getDepth() > configuration->dim.d)
        {
            chromozome[configuration->bitsPerItem * boxId[i] + (3 + 3 * configuration->coordinateBits)] = 0;
            isItemStillIn[i] = false;
            continue;
        }
        
        for (int j = i + 1; j < itemBoxes.size(); j++)
        {
            if (!isItemStillIn[j])
                continue;

            Box box = Box::intersect(itemBoxes[i], itemBoxes[j]);

            // calcuate how much volume is overlapping between boxes 
            if (box.getWidth() > 0 && box.getHeight() > 0 && box.getDepth() > 0)
            {
                std::uniform_int_distribution<int> whoToKeepDist(1,2);
                int keep = whoToKeepDist(randomEngine.getGenerator());
                if (keep == 1)
                {
                    chromozome[configuration->bitsPerItem * boxId[j] + (3 + 3 * configuration->coordinateBits)] = 0;
                    isItemStillIn[j] = false;
                }
                else
                {
                    chromozome[configuration->bitsPerItem * boxId[i] + (3 + 3 * configuration->coordinateBits)] = 0;
                    isItemStillIn[i] = false;
                    continue;
                }
            }
        }
    }

	if (applyDBLF)
	{
		//force every 1 out of 3 creatures to have dblf heuristics applied to it
		/*std::uniform_int_distribution<long unsigned int> trinDist(0, 1);
		int DBLFchance = trinDist(randomEngine.getGenerator());
		if (DBLFchance != 0) return;
		*/

		//seperete items indexes into two categories:taken and not taken 
		std::vector<int> itemsTakenIndexes;
		std::vector<int> itemsNotTakenIndexes;

		//do the seperation
		DynamicBitSet itemMask = configuration->itemMask;
		ItemInfo itemInfo;
		for (int i = 0; i < configuration->items.size(); i++)
		{
			if (getItemInfo(itemMask, i, itemInfo))
			{
				itemsTakenIndexes.push_back(i);
			}
			else
			{
				itemsNotTakenIndexes.push_back(i);
			}

			itemMask = itemMask << configuration->bitsPerItem;
		}
		
		//create a chromozome for dblf to work with
		std::vector<int> dblfChromozome;
		
		//the following code purpose is to make a dblf chromozome that will first take into account the
		//items that the binary creature decided to take and only then the rest of the items, all while giving some mixing criteria 

		//push all of the items
		for (int& takenIndex : itemsTakenIndexes)     dblfChromozome.push_back(takenIndex);
		for (int& takenIndex : itemsNotTakenIndexes)  dblfChromozome.push_back(takenIndex);
		

		

		//mix the placing order of the items that the binary creatures chose to take
		std::sort(dblfChromozome.begin(), dblfChromozome.begin() + itemsTakenIndexes.size());
		//todo: dont mix them up, and rather choose their ordering places based on how deep-bottom-left the binary creature chose
		//to place each item.
		//e.g: the item that the binary creature chose to be the deepestBottomLeft should be the first that dblf will try to place


		//mix the other items that the binary creature didnt take
		if (itemsTakenIndexes.size() != configuration->numberOfItems)
		{
			std::sort(dblfChromozome.begin() + itemsTakenIndexes.size() + 1, dblfChromozome.end());
		}
		//DBLF_core myDBLFHelper(configuration, dblfChromozome, dblfChromozome.size());
		DBLF_core myDBLFHelper(configuration, dblfChromozome, itemsTakenIndexes.size());
		std::vector<BoxInfo> dblfHelperResults = myDBLFHelper.getBoxPositions();

		
		for (BoxInfo& boxInfo : dblfHelperResults)
		{
			int id = boxInfo.id;
			std::vector<int> newGene;
			newGene.reserve(configuration->bitsPerItem);
			DynamicBitSet myX(configuration->coordinateBits, boxInfo.startingPoint.x());
			DynamicBitSet myY(configuration->coordinateBits, boxInfo.startingPoint.y());
			DynamicBitSet myZ(configuration->coordinateBits, boxInfo.startingPoint.z());

			for (int i = 0; i < configuration->coordinateBits; i++) newGene.push_back(myZ[i]);
			for (int i = 0; i < configuration->coordinateBits; i++) newGene.push_back(myY[i]);
			for (int i = 0; i < configuration->coordinateBits; i++) newGene.push_back(myX[i]);
			for (int i = 0; i < 3; i++) newGene.push_back(0);
			 newGene.push_back(1);
			
			int startPosition = id * configuration->bitsPerItem;
			for (int i = 0; i < configuration->bitsPerItem; i++)
			{
				chromozome[startPosition + i] = newGene[i];
			}
		}
	}


}
//Name: setSharedFitness
//-----------------------------------------------------------------------------------------------
void BinaryCreature::setSharedFitness(int newSharedFitness)
{
	sharedFitness = newSharedFitness;
}
//-----------------------------------------------------------------------------------------------
// Name : getSharedFitness
int BinaryCreature::getSharedFitness() const
{
	return sharedFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : adjustDimensionsToOrientation
// Output: the minDist for shared fitness
// Action: return the minDist for shared fitness based on the chromosome size
//-----------------------------------------------------------------------------------------------
unsigned int BinaryCreature::getMinDist()
{
    return chromozome.size() * 0.1f;
}

//-----------------------------------------------------------------------------------------------
// Name : adjustDimensionsToOrientation
// Input: item orientaion and refernces to its width,height and depth
// Output: width,height and depth swapped according to item orientaion
// Action: swaps width,height and depth based on the orientation value
//-----------------------------------------------------------------------------------------------
void BinaryCreature::adjustDimensionsToOrientation(int orientation, long unsigned int& width, long unsigned int& height, long unsigned int& depth)
{
    switch(orientation)
    {
        // Case 0:  X,Y,Z
        case 1: // Z,Y,X
        {
            std::swap(width, depth);
        }break;
                
        case 2: // Y,Z,X
        {
            std::swap(width, height);
            std::swap(height, depth);
        }break;
                
        case 3: // X,Z,Y
        {
            std::swap(height, depth);
        }break;
                
        case 4: // Y,X,Z
        {
            std::swap(width, height);
        }break;
                
        case 5: // Z,X,Y
        {
            std::swap(width, height);
            std::swap(width, depth);
        }break;
    }
}

//-----------------------------------------------------------------------------------------------
// Name : mutate
// Input: mutationChance - the chance for a bit to flip
// Output: the this chromozome is mutated
// Action: mutate the chromozome based on the chance of  mutationChance per bit to flip
//-----------------------------------------------------------------------------------------------
void BinaryCreature::mutate(float mutationChance, Random& randomEngine/* = Random::default_engine*/)
{
    std::vector<float> mutateVec = {mutationChance, 1 - mutationChance};

	std::uniform_real_distribution<> flipChance(0.01f, 0.08f);
	float singleBitflipRate = flipChance(randomEngine.getGenerator());

    std::vector<float> mutatePerBitVec = { singleBitflipRate, 1 - singleBitflipRate };
    std::discrete_distribution<int> mutateDist(mutateVec.begin(), mutateVec.end());
    std::discrete_distribution<int> mutatePerBitDist(mutatePerBitVec.begin(), mutatePerBitVec.end());
    
    unsigned int choromozomeSize = (unsigned int)chromozome.size();
   
    if (mutateDist(randomEngine.getGenerator()) == 0)
    {
        for (unsigned int i = 0; i <choromozomeSize; i++)
        {
            if (mutatePerBitDist(randomEngine.getGenerator()) == 0)
                chromozome.flip(i);
        }
    }
    repairChromosome(randomEngine);
}

//-----------------------------------------------------------------------------------------------
// Name : crossover
// Input: BinaryCreature to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using single point crossover
//-----------------------------------------------------------------------------------------------
void BinaryCreature::crossover(const BinaryCreature& parent2, std::vector<BinaryCreature>& population, Random& randomEngine/* = Random::default_engine*/) const
{
    //uniformCrossover(parent2, population, randomEngine);
    onePointCrossover(parent2, population, randomEngine);
}
//-----------------------------------------------------------------------------------------------
// Name : onePointCrossover
// Input: BinaryCreature to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using single point crossover
//-----------------------------------------------------------------------------------------------
void BinaryCreature::onePointCrossover(const BinaryCreature& parent2, std::vector<BinaryCreature>& population, Random& randomEngine) const
{
    const DynamicBitSet& parent1 = chromozome;
        
    std::uniform_int_distribution<int> joinBitDist(1, parent1.size() - 1);
    int joinBitLocation = (int)joinBitDist(randomEngine.getGenerator());
    joinBitLocation = (int)(parent1.size() - joinBitLocation);
    
    // mask starts as numberOfBits ones
    DynamicBitSet mask = DynamicBitSet(parent1.size(), 0);
    mask.flip();
    // leave ones in the mask only from joinBitLocation to LSB
    mask = mask >> joinBitLocation;
    DynamicBitSet flippedMask = mask;
    flippedMask.flip();
        
    DynamicBitSet chromozomeX = parent1 & flippedMask;      // upper  part
    DynamicBitSet chromozomeY = parent2.chromozome & mask;  // bottom part
    DynamicBitSet child = chromozomeX | chromozomeY;
    
    chromozomeX = parent2.chromozome & flippedMask;         // upper  part
    chromozomeY = parent1 & mask;                           // bottom part
    DynamicBitSet child2 = chromozomeX | chromozomeY;
    
    population.emplace_back(configuration, child);
    population.emplace_back(configuration, child2);
}
//-----------------------------------------------------------------------------------------------
// Name : uniformCrossover
// Input: BinaryCreature to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using unifrom crossover
//-----------------------------------------------------------------------------------------------
void BinaryCreature::uniformCrossover(const BinaryCreature& parent2, std::vector<BinaryCreature>& population, Random& randomEngine) const
{
    const DynamicBitSet& parent1 = chromozome;
    const DynamicBitSet& parent22 = parent2.chromozome;
    
    DynamicBitSet child = DynamicBitSet(chromozome.size(), 0);
    DynamicBitSet child2 = DynamicBitSet(chromozome.size(), 0);
    
    for (int i = 0; i < chromozome.size(); i++)
    {
        std::uniform_int_distribution<int> joinBitDist(1, 2);     
        if (joinBitDist(randomEngine.getGenerator()) == 1)
        {
            child[i] = parent1[i];
            //child2[i] = parent22[i];
        }
        else
        {
            child[i] = parent22[i];
            //child2[i] = parent1[i];
        }
        
        if (joinBitDist(randomEngine.getGenerator()) == 1)
        {
            child2[i] = parent1[i];
        }
        else
        {
            child2[i] = parent22[i];
        }
    }
    
    population.emplace_back(configuration, child);
    population.emplace_back(configuration, child2);
}

//-----------------------------------------------------------------------------------------------
// Name : calculateFittness
// Input: this chromozome
// Output: fitness score for this encdoing solution
// Action: calcuate the fitness score fro this encdoing
//         if there is no overlap between boxes thes score is 50% value and 50% positionScore
//         if there is overlap the score is only the positionScore
//         positionScore is made out of how much overlaping volume there was and how well
//         positioned are the boxes
//-----------------------------------------------------------------------------------------------
int BinaryCreature::calculateFittness()
{
	int nItem = 0;
	std::vector<Box> itemBoxes;
	std::vector<long int> valuesOfItems;
	int value = 0;
	long unsigned int minX = std::numeric_limits<long unsigned int>::max(),
	minY = std::numeric_limits<long unsigned int>::max(),
	minZ = std::numeric_limits<long unsigned int>::max();
	long unsigned int maxX = 0, maxY = 0, maxZ = 0;
    activeItemsMask = DynamicBitSet(chromozome.size(), 0);
    DynamicBitSet itemMask = configuration->itemMask;
	for (int i = 0; i < configuration->items.size(); i++)
	{
        ItemInfo itemInfo;
        if (getItemInfo(itemMask, i, itemInfo))
		{
            activeItemsMask = activeItemsMask | itemMask;
			nItem++;
			value += configuration->items[i].value;
			minX = std::min(minX, itemInfo.x);
			minY = std::min(minY, itemInfo.y);
			minZ = std::min(minZ, itemInfo.z);
			maxX = std::max(maxX, itemInfo.x + itemInfo.width);
			maxY = std::max(maxY, itemInfo.y + itemInfo.height);
			maxZ = std::max(maxZ, itemInfo.z + itemInfo.depth);

			itemBoxes.emplace_back(itemInfo.x, itemInfo.y, itemInfo.z,
                                   itemInfo.x + itemInfo.width,
                                   itemInfo.y + itemInfo.height,
                                   itemInfo.z + itemInfo.depth);
			valuesOfItems.push_back(configuration->items[i].value);
		}

		itemMask = itemMask << configuration->bitsPerItem;
	}

	int totalVolume = 0;
	bool overlapped = false;
	int overlappedVolume = 0;
	int cornerBonus = 0;
	int connectBonus = 0;
    int distance = 0;
	for (int i = 0; i < itemBoxes.size(); i++)
	{
		std::vector<Box> currentVolume;
		currentVolume.push_back(itemBoxes[i]);
		totalVolume += (itemBoxes[i].getWidth() * itemBoxes[i].getHeight() * itemBoxes[i].getDepth());

		for (int j = i + 1; j < itemBoxes.size(); j++)
		{
			Box box = Box::intersect(itemBoxes[i], itemBoxes[j]);

			// calcuate how much volume is overlapping between boxes 
			if (box.getWidth() > 0 && box.getHeight() > 0 && box.getDepth() > 0)
			{
				overlapped = true;
				overlappedVolume -= (box.getWidth() * box.getHeight() * box.getDepth() * penaltyWeight);
			}

			if (box.getWidth() == 0)
				connectBonus += valuesOfItems[i] / 4;
            
			if (box.getHeight() == 0)
				connectBonus += valuesOfItems[i] / 4;
            
			if (box.getDepth() == 0)
				connectBonus += valuesOfItems[i] / 4;
            
			int dWidth, dHieght, dLength;
			if (box.getWidth() < 0 || box.getHeight() < 0 || box.getDepth() < 0)
			{
				if (box.getWidth() != 0)
					dWidth = std::abs(box.getWidth());
				else
					dWidth = 1;
                
				if (box.getHeight() != 0)
					dHieght = std::abs(box.getHeight());
				else
					dHieght = 1;
                
				if (box.getDepth() != 0)
					dLength = std::abs(box.getDepth());
				else
					dLength = 1;
                
				int temp = (dWidth*dHieght*dLength);
				int temp2 = (configuration->dim.w*configuration->dim.h*configuration->dim.d);
				float temp3 = (float)temp / temp2;
				int temp4 = temp3 * valuesOfItems[j];
				distance -= temp4;
			}
		}
		
		if (configuration->dim.w - itemBoxes[i].right == 0)
			cornerBonus += valuesOfItems[i];
        
		if (itemBoxes[i].left == 0)
			cornerBonus += valuesOfItems[i];
        
		if (configuration->dim.h - itemBoxes[i].top == 0)
			cornerBonus += valuesOfItems[i];
        
		if (itemBoxes[i].bottom == 0)
			cornerBonus += valuesOfItems[i];
        
		if (configuration->dim.d - itemBoxes[i].front == 0)
			cornerBonus += valuesOfItems[i];
        
		if (itemBoxes[i].back == 0)
			cornerBonus += valuesOfItems[i];
	}

	fitness = value + cornerBonus + connectBonus / 4;
    
	return fitness;
}


//-----------------------------------------------------------------------------------------------
// Name : getBoxPositions
// Input: this chromozome
// Output: vector of the positions of the boxes inside the container
// Action: Decode the encdoing and returns where are the boxes inside the container
//-----------------------------------------------------------------------------------------------
std::vector<BoxInfo> BinaryCreature::getBoxPositions()
{   
	std::vector<BoxInfo> boxesPositions;
	std::vector<Item>& items = configuration->items;
	int value = 0;
	DynamicBitSet itemMask = configuration->itemMask;
	for (int i = 0; i < configuration->items.size(); i++)
	{
		ItemInfo itemInfo;
		if (getItemInfo(itemMask, i, itemInfo))
		{
			value += items[i].value;
			// get Item Dimension
			boxesPositions.emplace_back(QPoint3D(itemInfo.x, itemInfo.y, itemInfo.z),
                                        RGB(items[i].color.r / 256.0f,
                                            items[i].color.g / 256.0f,
                                            items[i].color.b / 256.0f),
                                        itemInfo.width,
                                        itemInfo.height,
                                        itemInfo.depth,
                                        items[i].value,
										items[i].id);
		}
		itemMask = itemMask << configuration->bitsPerItem;
	}
	return boxesPositions;
}
//-----------------------------------------------------------------------------------------------
// Name : getConfiguration
// Action: return the configuration used in this encdoing
//-----------------------------------------------------------------------------------------------
Configuration* BinaryCreature::getConfiguration() const
{
    return this->configuration;
}
//-----------------------------------------------------------------------------------------------
// Name : validateConstraints
// Action: checks if problem Constraint are violated if there are increase w
//-----------------------------------------------------------------------------------------------
bool BinaryCreature::validateConstraints()
{
	std::vector<Box> itemBoxes;
	std::vector<long int> valuesOfItems;
	int value = 0;
	DynamicBitSet itemMask = configuration->itemMask;
	for (int i = 0; i < configuration->items.size(); i++)
	{
		ItemInfo itemInfo;
		if (getItemInfo(itemMask, i, itemInfo))
		{
			value += configuration->items[i].value;
			itemBoxes.emplace_back(itemInfo.x, itemInfo.y, itemInfo.z,
                                   itemInfo.x + itemInfo.width,
                                   itemInfo.y + itemInfo.height,
                                   itemInfo.z + itemInfo.depth);
			valuesOfItems.push_back(configuration->items[i].value / (itemInfo.width * itemInfo.height * itemInfo.depth));
		}

		itemMask = itemMask << configuration->bitsPerItem;
	}

	for (int i = 0; i < itemBoxes.size(); i++)
	{
		for (int j = i + 1; j < itemBoxes.size(); j++)
		{
			Box box = Box::intersect(itemBoxes[i], itemBoxes[j]);

			// calcuate how much volume is overlapping between boxes 
			if (box.getWidth() > 0 && box.getHeight() > 0 && box.getDepth() > 0)
			{
				penaltyWeight += 10;
				return false;
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------
// Name : hammingDistance
// calculate the hamming distance between two creatures by comparing the bits within the items that each creature takes.
// This is better than comparing all of their bits, since  two creatures may differentiate mostly by the bits of the items which
// they dont take into the container, and thus their normal hamming distance be quite large, even though they are the same.
int BinaryCreature::hammingDistance(BinaryCreature& other)
{
    DynamicBitSet sharedMask = activeItemsMask | other.activeItemsMask;
    DynamicBitSet diffBits = chromozome ^ other.chromozome;
    diffBits = diffBits & sharedMask;
    
	return diffBits.count();
}

//-----------------------------------------------------------------------------------------------
// Name : setFitness
// sets the value of the fitness
//-----------------------------------------------------------------------------------------------
void BinaryCreature::setFitness(int newFitness)
{
    fitness = newFitness;
}

//-----------------------------------------------------------------------------------------------
// Name : getFitness
// Action: return the fitness of this encdoing
//-----------------------------------------------------------------------------------------------
int BinaryCreature::getFitness() const
{
    return fitness;
}
//-----------------------------------------------------------------------------------------------
// Name : getItemInfo
// input: itemMask for the item in the itemMask position, index of item inside the chromozome
//        and a ItemInfo struct to hold the itemInfo data
// output: true if the item is in the container and its info in iteminfo otherwise false
// Action: return the info of the item in the itemIndex place
//-----------------------------------------------------------------------------------------------
bool BinaryCreature::getItemInfo(DynamicBitSet& itemMask, int itemIndex, ItemInfo& itemInfo)
{
    // get the item bits 
    DynamicBitSet itemValues = chromozome & itemMask;
    // shift them to lower parts of the bit string
    itemValues = itemValues >> configuration->bitsPerItem * itemIndex;
    if (itemValues[3 + 3 * configuration->coordinateBits])
    {
        // get the X,Y,Z coordinates
        itemInfo.x = ((itemValues & configuration->xMask) >> configuration->coordinateBits * 2).to_ulong();
        itemInfo.y = ((itemValues & configuration->yMask) >> configuration->coordinateBits).to_ulong();
        itemInfo.z = (itemValues & configuration->zMask).to_ulong();
        // get Item Dimension
        itemInfo.width = configuration->items[itemIndex].dim.w;
        itemInfo.height = configuration->items[itemIndex].dim.h;
        itemInfo.depth = configuration->items[itemIndex].dim.d;
        DynamicBitSet orientationBits = (itemValues >> 3 * configuration->coordinateBits) &
                                        configuration->sevenMask;
        itemInfo.orientaion = orientationBits.to_ulong();        
        adjustDimensionsToOrientation(itemInfo.orientaion, itemInfo.width, itemInfo.height,
                                      itemInfo.depth);        
        return true;
    }
    else
        return false;
}
