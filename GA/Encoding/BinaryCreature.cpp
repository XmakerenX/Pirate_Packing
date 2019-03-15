#include "BinaryCreature.h"

//-----------------------------------------------------------------------------------------------
// Name : BinaryCreature
// Input: Configuration of the problem 
// Output: A random Binary encdoing for a solution for the problem
// Action: Creates a random Binary encdoing for a solution for the given problem
//-----------------------------------------------------------------------------------------------
BinaryCreature::BinaryCreature(Configuration* conf)
    :configuration(conf)
{
    long unsigned int maxDimensionValue = std::max(conf->dim.w, conf->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, conf->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    // 1 bit for if item is in container
    // 3 bits for the item orientaion in the container
    // X bits for each coordinate X,Y,Z depend on the container max Dimension value
    // Binary string will look like this : 1 333 XXXX XXXX XXXX
    long unsigned int totalBitsNum = configuration->items.size()*4 + 3 * configuration->items.size() * coordinateBits;
    chromozome = generateChromosome(totalBitsNum);
    repairChromosome();
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
// Name : generateChromosome
// Input: totalBitsNum - number of bits for the chromosome
// Output: A random DynamicBitSet of size totalBitsNum
// Action: Creates a random DynamicBitSet with size totalBitsNum
//-----------------------------------------------------------------------------------------------
DynamicBitSet BinaryCreature::generateChromosome(long unsigned int totalBitsNum)
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
            chromosomeInput.push_back(longDistributaion(Random::default_engine.getGenerator()));
            curTotalBitsNum -= 64;
        }
        else
        {
            long unsigned int reminderMaxNum = std::pow(2, curTotalBitsNum);
            reminderMaxNum--;
            std::uniform_int_distribution<long unsigned int> reminderDistributaion(0, reminderMaxNum);
            chromosomeInput.push_back(reminderDistributaion(Random::default_engine.getGenerator()));
            curTotalBitsNum -= curTotalBitsNum;
        }
    }
    
    DynamicBitSet chromosome = DynamicBitSet(chromosomeInput.begin(), chromosomeInput.end());
    // the chromosome size will be in multiplication of 64 so shirnk it to totalBitsNum
    // to avoid usesless zeros at the binary string start
    chromosome.resize(totalBitsNum);
    
    return chromosome;
}

//-----------------------------------------------------------------------------------------------
// Name : repairChromosome
// Input: this encdoing chromozome with possible invalid values
// Output: this encdoing chromozome with valid values
// Action: checks for invalid values in chromozome amd fixes them
// * invalid values are X,Y,Z that puts the item outside the container
//-----------------------------------------------------------------------------------------------
void BinaryCreature::repairChromosome()
{
    
    // get how many bits are used to hold the coordinate number
    long unsigned int maxDimensionValue = std::max(configuration->dim.w, configuration->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, configuration->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    long unsigned int bitsPerItem = 4 + 3 * coordinateBits;
    
    for (int i = 0; i < configuration->items.size(); i++)
    {
        // craete mask to get current item info from the chromosome
        unsigned int itemMaskValue = std::pow(2, bitsPerItem);
        itemMaskValue--;
        DynamicBitSet itemMask =  DynamicBitSet(chromozome.size(), itemMaskValue);
        itemMask = itemMask << bitsPerItem * i;
        // get the item info from the chromosome
        DynamicBitSet itemValues = chromozome & itemMask;
        itemValues = itemValues >> bitsPerItem * i;
        // check if the item is in selected to be in the container
        if (itemValues[3 + 3 * coordinateBits] == 1)
        {
            // create mask to get the y value from the item bit string
            unsigned int zMaskValue = std::pow(2, coordinateBits);
            zMaskValue--;
            // get the item z coordinate
            DynamicBitSet zMask = DynamicBitSet(chromozome.size(), zMaskValue);
            unsigned int z = (itemValues & zMask).to_ulong();
            // get the item y coordinate
            DynamicBitSet yMask = zMask << coordinateBits;
            DynamicBitSet yValue = (itemValues & yMask) >> coordinateBits;
            unsigned int y = yValue.to_ulong();
            // create mask to get the x value from the item bit string
            DynamicBitSet xMask = yMask << coordinateBits;
            DynamicBitSet xValue = (itemValues & xMask) >> 2*coordinateBits;
            // get the item x coordinate
            unsigned int x = xValue.to_ulong();
            long unsigned int itemWidth = configuration->items[i].dim.w;
            long unsigned int itemHeight = configuration->items[i].dim.h;
            long unsigned int itemDepth  = configuration->items[i].dim.d;
            // swap item width and height if item is vertical orientation
            DynamicBitSet seven(itemValues.size(), 7);
            DynamicBitSet orientationBits = (itemValues >> 3*coordinateBits) & seven;
            int orientaion = orientationBits.to_ulong();
            // make sure orientaion is in the valid range
            if (orientaion > 5)
            {
                std::uniform_int_distribution<unsigned int> orientaionDist(0, 5);
                orientaion = orientaionDist(Random::default_engine.getGenerator());
            }
            adjustDimensionsToOrientation(orientaion, itemWidth, itemHeight, itemDepth);
            
                        
            // repair if needed the x and y coordinates
            if (x + itemWidth > configuration->dim.w)
                x = configuration->dim.w - itemWidth;
            if (y + itemHeight > configuration->dim.h)
                y = configuration->dim.h - itemHeight;
            if (z + itemDepth > configuration->dim.d)
                z = configuration->dim.d - itemDepth;
            
            DynamicBitSet newOrientation(chromozome.size(), orientaion);
            newOrientation << coordinateBits*3;
            DynamicBitSet newX(chromozome.size(), x);
            newX = newX << coordinateBits*2;
            DynamicBitSet newY(chromozome.size(), y);
            newY = newY << coordinateBits;
            DynamicBitSet newZ(chromozome.size(), z);
            DynamicBitSet updatedXYValue = newOrientation | newX | newY | newZ;
            // copy the updated value to chromosome
            for (int j = 0; j < (bitsPerItem - 1); j++)
            {
                chromozome[bitsPerItem * i + j] = updatedXYValue[j];
            }
        }
    }
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
void BinaryCreature::mutate(float mutationChance)
{
    std::vector<float> mutateVec = {mutationChance, 1 - mutationChance};
    std::discrete_distribution<int> mutateDist(mutateVec.begin(), mutateVec.end());
    
	unsigned int choromozomeSize = chromozome.size();
   
	for (int i = 0; i <choromozomeSize; i++)
    {
        if (mutateDist(Random::default_engine.getGenerator()) == 0)
            chromozome.flip(i);
    }
    repairChromosome();
}

//-----------------------------------------------------------------------------------------------
// Name : crossover
// Input: BinaryCreature to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using single point crossover
//-----------------------------------------------------------------------------------------------
void BinaryCreature::crossover(BinaryCreature parent2, std::vector<BinaryCreature>& population)
{
    DynamicBitSet& parent1 = chromozome;
    DynamicBitSet& parent22 = parent2.chromozome;
    
    std::uniform_int_distribution<int> joinBitDist(1, parent1.size() - 1);
    int joinBitLocation = joinBitDist(Random::default_engine.getGenerator());
    //int joinBitLocation = (rand() % parent1.size());
    joinBitLocation = parent1.size() - joinBitLocation;
    
    // mask starts as numberOfBits ones
    unsigned long temp = pow(2, parent1.size() + 1) - 1;
    DynamicBitSet mask = DynamicBitSet(parent1.size(), temp);
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
    // get how many bits are used to hold the coordinate number
    long unsigned int maxDimensionValue = std::max(configuration->dim.w, configuration->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, configuration->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    long unsigned int bitsPerItem = 4 + 3 * coordinateBits;
    
    long unsigned int itemMaskValue = std::pow(2, bitsPerItem);
    itemMaskValue--;
    DynamicBitSet itemMask = DynamicBitSet(chromozome.size(), itemMaskValue);
    
    std::vector<Box> itemBoxes;
    int value = 0;
    for (int i = 0; i < configuration->items.size(); i++)
    {
        // get the item bits 
        DynamicBitSet itemValues = chromozome & itemMask;
        // shift them to lower parts of the bit string
        itemValues = itemValues >> bitsPerItem * i;
        if (itemValues[3 + 3 * coordinateBits] == 1)
        {
            value += configuration->items[i].value;
            // get the X,Y,Z coordinates
            long unsigned int zMaskValue = std::pow(2, coordinateBits);
            zMaskValue--;
            DynamicBitSet zMask = DynamicBitSet(chromozome.size(), zMaskValue);
            long unsigned int z = (itemValues & zMask).to_ulong();
            DynamicBitSet yMask = zMask << coordinateBits;
            long unsigned int y = ((itemValues & yMask) >> coordinateBits).to_ulong();
            DynamicBitSet xMask = yMask << coordinateBits;
            long unsigned int x = ((itemValues & xMask) >> coordinateBits*2).to_ulong();
            // get Item Dimension
            long unsigned int itemWidth = configuration->items[i].dim.w;
            long unsigned int itemHeight = configuration->items[i].dim.h;
            long unsigned int itemDepth = configuration->items[i].dim.d;
            
            DynamicBitSet seven(itemValues.size(), 7);
            DynamicBitSet orientationBits = (itemValues >> 3*coordinateBits) & seven;
            int orientaion = orientationBits.to_ulong();
            adjustDimensionsToOrientation(orientaion, itemWidth, itemHeight, itemDepth);
            itemBoxes.emplace_back(x,y,z, x + itemWidth, y + itemHeight, z + itemDepth);
        }
               
        itemMask = itemMask << bitsPerItem;
    }
    
    bool overlapped = false;
    int overlappedVolume = 0;
    int positionScore = 0;
    for (int i = 0; i < itemBoxes.size(); i++)
    {
        for (int j = i + 1; j < itemBoxes.size(); j++)
        {
            Box box = Box::intersect(itemBoxes[i], itemBoxes[j]);
            
            // calcuate how much volume is overlapping between boxes
            if (box.getWidth() > 0 && box.getHeight() > 0 && box.getDepth() > 0)
            {
                overlapped = true;
                overlappedVolume -= box.getWidth() * box.getHeight() * box.getDepth();
            }
            
            // calcuate how much free spapce is between boxes
            if (box.getWidth() < 0 && box.getHeight() < 0 && box.getDepth() < 0)
            {
                positionScore -= box.getWidth() * box.getHeight() * box.getDepth();
            }
            
            // give bonus for boxes that have no space between them
            if (box.getWidth() == 0 || box.getHeight() == 0 || box.getDepth() < 0)
            {
                positionScore += (box.getWidth() * box.getHeight() * box.getDepth())/6;
            }
        }
    }
    
    if (!overlapped)
        fitness = value*0.5 + positionScore*0.5;
    else
        fitness =  overlappedVolume*0.5 + positionScore*0.5;
    
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
    // get how many bits are used to hold the coordinate number
    long unsigned int maxDimensionValue = std::max(configuration->dim.w, configuration->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, configuration->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    long unsigned int bitsPerItem = 4 + 3 * coordinateBits;
    
    long unsigned int itemMaskValue = std::pow(2, bitsPerItem);
    itemMaskValue--;
    DynamicBitSet itemMask = DynamicBitSet(chromozome.size(), itemMaskValue);
    
    std::vector<BoxInfo> itemInfo;
    std::vector<Item>& items = configuration->items;
    int value = 0;
    for (int i = 0; i < configuration->items.size(); i++)
    {
        // get the item bits 
        DynamicBitSet itemValues = chromozome & itemMask;
        // shift them to lower parts of the bit string
        itemValues = itemValues >> bitsPerItem * i;
        if (itemValues[3 + 3 * coordinateBits] == 1)
        {
            value += items[i].value;
            // get the X,Y,Z coordinates
            long unsigned int zMaskValue = std::pow(2, coordinateBits);
            zMaskValue--;
            DynamicBitSet zMask = DynamicBitSet(chromozome.size(), zMaskValue);
            long unsigned int z = (itemValues & zMask).to_ulong();
            DynamicBitSet yMask = zMask << coordinateBits;
            long unsigned int y = ((itemValues & yMask) >> coordinateBits).to_ulong();
            DynamicBitSet xMask = yMask << coordinateBits;
            long unsigned int x = ((itemValues & xMask) >> coordinateBits*2).to_ulong();
            // get Item Dimension
            long unsigned int itemWidth =items[i].dim.w;
            long unsigned int itemHeight = items[i].dim.h;
            long unsigned int itemDepth = items[i].dim.d;
            
            DynamicBitSet seven(itemValues.size(), 7);
            DynamicBitSet orientationBits = (itemValues >> 3*coordinateBits) & seven;
            int orientaion = orientationBits.to_ulong();
            adjustDimensionsToOrientation(orientaion, itemWidth, itemHeight, itemDepth);
            itemInfo.emplace_back(QPoint3D(x,y,z),
                                  RGB(items[i].color.r / 256.0f,
                                      items[i].color.g / 256.0f,
                                      items[i].color.b / 256.0f),
                                  itemWidth,
                                  itemHeight,
                                  itemDepth);
        }
               
        itemMask = itemMask << bitsPerItem;
    }
    
    return itemInfo;
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
