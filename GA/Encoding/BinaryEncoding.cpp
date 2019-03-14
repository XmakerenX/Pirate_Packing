#include "BinaryEncoding.h"

//-----------------------------------------------------------------------------------------------
// BinaryEncoding
// Input: Configuration of the problem 
// Output: A random Binary encdoing for a solution for the problem
// Action: Creates a random Binary encdoing for a solution for the given problem
//-----------------------------------------------------------------------------------------------
BinaryEncoding::BinaryEncoding(Configuration* conf)
    :configuration(conf)
{
    long unsigned int maxDimensionValue = std::max(conf->container_width, conf->container_height, conf->container_depth);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    // 1 bit for if item is in container
    // 3 bits for the item orientaion in the container
    // X bits for each coordinate X,Y,Z depend on the container max Dimension value
    // Binary string will look like this : 1 333 XXXX XXXX XXXX
    long unsigned int totalBitsNum = configuration->items.size()*4 + 3 * configuration->items.size() * coordinateBits;
    chromozome = generateChromosome(totalBitsNum);
}

//-----------------------------------------------------------------------------------------------
// BinaryEncoding
// Input: Configuration of the problem and the encdoing chromozome
// Output: Binary encdoing built with the given chromozome
// Action:  Creates a Binary encdoing with the given chromozome
// * copies the _chromozome
//-----------------------------------------------------------------------------------------------
BinaryEncoding::BinaryEncoding(Configuration* config, const DynamicBitSet& _chromozome)
    :chromozome(_chromozome), configuration(config)
{
    
}

//-----------------------------------------------------------------------------------------------
// BinaryEncoding
// Input: Configuration of the problem and the encdoing chromozome
// Output: Binary encdoing built with the given chromozome
// Action:  Creates a Binary encdoing with the given chromozome
// * moves the _chromozome
//-----------------------------------------------------------------------------------------------
BinaryEncoding::BinaryEncoding(Configuration* config, DynamicBitSet&& _chromozome)
    :chromozome(std::move(_chromozome)), configuration(config)
{
    
}

//-----------------------------------------------------------------------------------------------
// generateChromosome
// Input: totalBitsNum - number of bits for the chromosome
// Output: A random DynamicBitSet of size totalBitsNum
// Action: Creates a random DynamicBitSet with size totalBitsNum
//-----------------------------------------------------------------------------------------------
DynamicBitSet BinaryEncoding::generateChromosome(long unsigned int totalBitsNum)
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
// repairChromosome
// Input: this encdoing chromozome with possible invalid values
// Output: this encdoing chromozome with valid values
// Action: checks for invalid values in chromozome amd fixes them
// * invalid values are X,Y,Z that puts the item outside the container
//-----------------------------------------------------------------------------------------------
void BinaryEncoding::repairChromosome()
{
    
    // get how many bits are used to hold the coordinate number
    long unsigned int coordinateBits = std::ceil(std::log2(std::max(configuration->container_width, 
                                                                    configuration->container_height,
                                                                    configuration->container_depth)));
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
            unsigned int itemWidth = configuration->items[i].width;
            unsigned int itemHeight = configuration->items[i].height;
            unsigned int itemDepth  = configuration->items[i].depth;
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
            switch(orientaion)
            {
                // Case 0:  X,Y,Z
                case 1: // Z,Y,X
                {
                    std::swap(itemWidth, itemDepth);
                }break;
                
                case 2: // Y,Z,X
                {
                    std::swap(itemWidth, itemHeight);
                    std::swap(itemHeight, itemDepth);
                }break;
                
                case 3: // X,Z,Y
                {
                    std::swap(itemHeight, itemDepth);
                }break;
                
                case 4: // Y,X,Z
                {
                    std::swap(itemWidth, itemHeight);
                }break;
                
                case 5: // Z,X,Y
                {
                    std::swap(itemWidth, itemHeight);
                    std::swap(itemWidth, itemDepth);
                }break;
            }
            
                        
            // repair if needed the x and y coordinates
            if (x + itemWidth > configuration->container_width)
                x = configuration->container_width - itemWidth;
            if (y + itemHeight > configuration->container_height)
                y = configuration->container_height - itemHeight;
            if (z + itemDepth > configuration->container_depth)
                z = configuration->container_depth - itemDepth;
            
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
// mutate
// Input: mutationChange - the chance for a bit to flip
// Output: the this chromozome is mutated
// Action: mutate the chromozome based on the chance of  mutationChange per bit to flip
//-----------------------------------------------------------------------------------------------
void BinaryEncoding::mutate(float mutationChange)
{
    std::vector<float> mutateVec = {mutationChange, 1 - mutationChange};
    std::discrete_distribution<int> mutateDist(mutateVec.begin(), mutateVec.end());
    
    for (int i = 0; i < chromozome.size(); i++)
    {
        if (mutateDist(Random::default_engine.getGenerator()) == 0)
            chromozome.flip(i);
    }
}

//-----------------------------------------------------------------------------------------------
// crossover
// Input: BinaryEncoding to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using single point crossover
//-----------------------------------------------------------------------------------------------
std::vector<BinaryEncoding> BinaryEncoding::crossover(BinaryEncoding parent2)
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
        
    DynamicBitSet chromozomeX = parent1 & flippedMask; // upper  part
    DynamicBitSet chromozomeY = parent22 & mask;          // bottom part
    DynamicBitSet child = chromozomeX | chromozomeY;
    //mutateChild(child);
    
    chromozomeX = parent22 & flippedMask; // upper  part
    chromozomeY = parent1 & mask;          // bottom part
    DynamicBitSet child2 = chromozomeX | chromozomeY;
    //mutateChild(child2);
    
    std::vector<BinaryEncoding> children;
    children.emplace_back(configuration, child);
    children.emplace_back(configuration, child2);
    
    return children;
}

int BinaryEncoding::calculateFittness()
{
    return 0;
}

std::vector<BoxInfo> BinaryEncoding::getBoxPositions()
{
    return std::vector<BoxInfo>();
}

Configuration* BinaryEncoding::getConfiguration() const
{
    return this->configuration;
}

