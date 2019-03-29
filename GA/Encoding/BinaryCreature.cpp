#include "BinaryCreature.h"

int BinaryCreature::penaltyWeight = 1;

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
    
    long unsigned int maxDimensionValue = std::max(configuration->dim.w, configuration->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, configuration->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    long unsigned int bitsPerItem = 4 + 3 * coordinateBits;
    DynamicBitSet itemMask = DynamicBitSet(chromosome.size(), 0x8000 );
    for (int i = 0; i < configuration->items.size() - 1; i++)
    {
        itemMask = itemMask << 1;
        itemMask[0] = 1;
        itemMask = itemMask << (bitsPerItem - 1);
    }
    
//     // limit the number of items per chromosome
//     std::uniform_int_distribution<long unsigned int> itemDistributaion(0, configuration->items.size() - 1);
//     for (int i = 0; i < 10; i++)
//     {
//         itemMask[itemDistributaion(Random::default_engine.getGenerator()) * bitsPerItem + (3 + 3 * coordinateBits)] = 0;
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
void BinaryCreature::repairChromosome()
{
    
    // get how many bits are used to hold the coordinate number
    long unsigned int maxDimensionValue = std::max(configuration->dim.w, configuration->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, configuration->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    long unsigned int bitsPerItem = 4 + 3 * coordinateBits;
    std::vector<Box> itemBoxes;
    std::vector<long int> valuesOfItems;
    std::vector<long int> boxId;
    std::vector<bool> isItemStillIn;
    
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
            newOrientation = newOrientation << coordinateBits*3;
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
            
            itemBoxes.emplace_back(x,y,z, x + itemWidth, y + itemHeight, z + itemDepth);
            valuesOfItems.push_back(configuration->items[i].value);
            boxId.push_back(i);
            isItemStillIn.push_back(true);
        }
    }
    
    // remove conflicts based on value
    for (int i = 0; i < itemBoxes.size(); i++)
    {       
        if (!isItemStillIn[i])
                continue;
        
        for (int j = i + 1; j < itemBoxes.size(); j++)
        {
            if (!isItemStillIn[j])
                continue;
            
            Box box = Box::intersect(itemBoxes[i], itemBoxes[j]);
            
            // calcuate how much volume is overlapping between boxes 
            if (box.getWidth() > 0 && box.getHeight() > 0 && box.getDepth() > 0)
            {
                if (valuesOfItems[i] > valuesOfItems[j])
                {
                    chromozome[bitsPerItem * boxId[j] + (3 + 3 * coordinateBits)] = 0;
                    isItemStillIn[j] = false;
                }
                else
                {
                    chromozome[bitsPerItem * boxId[i] + (3 + 3 * coordinateBits)] = 0;
                    isItemStillIn[i] = false;
                    continue;
                }
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
    std::vector<float> mutatePerBitVec = {0.01f, 1 - 0.01f};
    std::discrete_distribution<int> mutateDist(mutateVec.begin(), mutateVec.end());
    std::discrete_distribution<int> mutatePerBitDist(mutatePerBitVec.begin(), mutatePerBitVec.end());
    
    unsigned int choromozomeSize = chromozome.size();
   
    if (mutateDist(Random::default_engine.getGenerator()) == 0)
    {
        for (int i = 0; i <choromozomeSize; i++)
        {
            if (mutatePerBitDist(Random::default_engine.getGenerator()) == 0)
                chromozome.flip(i);
        }
    }
    repairChromosome();
}

//-----------------------------------------------------------------------------------------------
// Name : crossover
// Input: BinaryCreature to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using single point crossover
//-----------------------------------------------------------------------------------------------
void BinaryCreature::crossover(BinaryCreature& parent2, std::vector<BinaryCreature>& population)
{
    //uniformCrossover(parent2, population);
    onePointCrossover(parent2, population);
}
//-----------------------------------------------------------------------------------------------
// Name : onePointCrossover
// Input: BinaryCreature to make the corssover with
// Output: vector with 2 children made from the 2 parents
// Action: Creates 2 children from this and parent2 using single point crossover
//-----------------------------------------------------------------------------------------------
void BinaryCreature::onePointCrossover(BinaryCreature& parent2, std::vector<BinaryCreature>& population)
{
    DynamicBitSet& parent1 = chromozome;
        
    std::uniform_int_distribution<int> joinBitDist(1, parent1.size() - 1);
    int joinBitLocation = joinBitDist(Random::default_engine.getGenerator());
    joinBitLocation = parent1.size() - joinBitLocation;
    
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
void BinaryCreature::uniformCrossover(BinaryCreature& parent2, std::vector<BinaryCreature>& population)
{
    DynamicBitSet& parent1 = chromozome;
    DynamicBitSet& parent22 = parent2.chromozome;
    
    DynamicBitSet child = DynamicBitSet(chromozome.size(), 0);
    DynamicBitSet child2 = DynamicBitSet(chromozome.size(), 0);
    
    for (int i = 0; i < chromozome.size(); i++)
    {
        std::uniform_int_distribution<int> joinBitDist(1, 2);     
        if (joinBitDist(Random::default_engine.getGenerator()) == 1)
        {
            child[i] = parent1[i];
            //child2[i] = parent22[i];
        }
        else
        {
            child[i] = parent22[i];
            //child2[i] = parent1[i];
        }
        
        if (joinBitDist(Random::default_engine.getGenerator()) == 1)
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
    // get how many bits are used to hold the coordinate number
    long unsigned int maxDimensionValue = std::max(configuration->dim.w, configuration->dim.h);
    maxDimensionValue = std::max(maxDimensionValue, configuration->dim.d);
    long unsigned int coordinateBits = std::ceil(std::log2(maxDimensionValue));
    long unsigned int bitsPerItem = 4 + 3 * coordinateBits;
    
    long unsigned int itemMaskValue = std::pow(2, bitsPerItem);
    itemMaskValue--;
    DynamicBitSet itemMask = DynamicBitSet(chromozome.size(), itemMaskValue);
    
    std::vector<Box> itemBoxes;
    std::vector<long int> valuesOfItems;
    int value = 0;
    long unsigned int minX = maxDimensionValue, minY = maxDimensionValue, minZ = maxDimensionValue;
    long unsigned int maxX = 0 , maxY = 0, maxZ = 0;
    for (int i = 0; i < configuration->items.size(); i++)
    {
        // get the item bits 
        DynamicBitSet itemValues = chromozome & itemMask;
        // shift them to lower parts of the bit string
        itemValues = itemValues >> bitsPerItem * i;
        if (itemValues[3 + 3 * coordinateBits] == 1)
        {
            nItem++;
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
            
            minX = std::min(minX, x); minY = std::min(minY, y); minZ = std::min(minZ, z);
            maxX = std::max(maxX, x + itemWidth);
            maxY = std::max(maxY, y + itemHeight);
            maxZ = std::max(maxZ, z + itemDepth);
            
            itemBoxes.emplace_back(x,y,z, x + itemWidth, y + itemHeight, z + itemDepth);
            valuesOfItems.push_back(configuration->items[i].value / (itemWidth * itemHeight * itemDepth));
        }
               
        itemMask = itemMask << bitsPerItem;
    }
    
    int totalVolume = 0;
    bool overlapped = false;
    int overlappedVolume = 0;
    int positionScore = 0;
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
                overlappedVolume -= (box.getWidth() * box.getHeight() * box.getDepth() * penaltyWeight );
            }
            
            // calcuate how much free spapce is between boxes
            if (box.getWidth() < 0 && box.getHeight() < 0 && box.getDepth() < 0)
            {
                positionScore -= std::abs(box.getWidth()) * std::abs(box.getHeight()) * std::abs(box.getDepth());
            }
            
            // give bonus for boxes that have no space between them
            if (box.getWidth() == 0 && box.getHeight() <= 0 && box.getDepth() <= 0)
                positionScore += (std::abs(box.getHeight()) * std::abs(box.getDepth()));
                
            if ((box.getWidth() <= 0 && box.getHeight() == 0 && box.getDepth() <= 0))
                positionScore += (std::abs(box.getWidth()) * std::abs(box.getDepth()));
            
            if ((box.getWidth() <= 0 && box.getHeight() <= 0 && box.getDepth() == 0))
                positionScore += (std::abs(box.getWidth()) * std::abs(box.getHeight())); 
            
            //---------------------------------------------------------------------------
            if (box.getWidth() == 0 && box.getHeight() == 0 && box.getDepth() <= 0)
                positionScore += (std::abs(box.getDepth()) * std::abs(box.getDepth()));
            
            if (box.getWidth() == 0 && box.getDepth() == 0 && box.getHeight() <= 0)
                positionScore += (std::abs(box.getHeight()) * std::abs(box.getHeight()));
            
            if (box.getHeight() == 0 && box.getDepth() == 0 && box.getWidth() <= 0)
                positionScore += (std::abs(box.getWidth()) * std::abs(box.getWidth()));
        }
        
        Box container(0,0,0, configuration->dim.w, configuration->dim.h, configuration->dim.d);
        Box box2 = Box::intersect(container, itemBoxes[i]);
//         if (box2.getWidth() > 0 && box2.getHeight() > 0 && box2.getDepth() > 0)
//         {
//             overlappedVolume += (box2.getWidth() * box2.getHeight() * box2.getDepth() * valuesOfItems[i]);
//         }
        
        if (box2.getWidth() < 0 && box2.getHeight() < 0 && box2.getDepth() < 0)
        {
            positionScore -= std::abs(box2.getWidth()) * std::abs(box2.getHeight()) * std::abs(box2.getDepth());
        }
        
        if (box2.getWidth() == 0 && box2.getHeight() <= 0 && box2.getDepth() <= 0)
            positionScore += std::abs(box2.getHeight()) * std::abs(box2.getDepth());
                
        if ((box2.getWidth() <= 0 && box2.getHeight() == 0 && box2.getDepth() <= 0))
            positionScore += std::abs(box2.getWidth()) * std::abs(box2.getDepth());
            
        if ((box2.getWidth() <= 0 && box2.getHeight() <= 0 && box2.getDepth() == 0))
            positionScore += std::abs(box2.getWidth()) * std::abs(box2.getHeight());
        
        if (box2.getWidth() == 0 && box2.getHeight() == 0 && box2.getDepth() <= 0)
            positionScore += (std::abs(box2.getDepth()) * std::abs(box2.getDepth()));
            
        if (box2.getWidth() == 0 && box2.getDepth() == 0 && box2.getHeight() <= 0)
            positionScore += (std::abs(box2.getHeight()) * std::abs(box2.getHeight()));
            
        if (box2.getHeight() == 0 && box2.getDepth() == 0 && box2.getWidth() <= 0)
            positionScore += (std::abs(box2.getWidth()) * std::abs(box2.getWidth()));
    }
    
    int compressedVolume = (configuration->dim.w * configuration->dim.h * configuration->dim.d) - 
                           ((maxX - minX) * (maxY - minY) * (maxZ - minZ));

    fitness =  0.4*(value) + 0.3 * (positionScore) + 0.3*overlappedVolume + compressedVolume;
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
                                  itemDepth,
                                  items[i].value);
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
// Name : validateConstraints
// Action: checks if problem Constraint are violated if there are increase w
//-----------------------------------------------------------------------------------------------
bool BinaryCreature::validateConstraints()
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
    std::vector<long int> valuesOfItems;
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
            valuesOfItems.push_back(configuration->items[i].value / (itemWidth * itemHeight * itemDepth));
        }
               
        itemMask = itemMask << bitsPerItem;
    }
    
    for (int i = 0; i < itemBoxes.size(); i++)
    {
        std::vector<Box> currentVolume;
        currentVolume.push_back(itemBoxes[i]);
        
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
