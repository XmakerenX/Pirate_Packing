#ifndef BINARYENCODING_H
#define BINARYENCODING_H

#include <cmath>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include "../Configuration.h"

typedef boost::dynamic_bitset<> DynamicBitSet;

class BinaryEncoding 
{
public:
    BinaryEncoding(Configuration* config);
    BinaryEncoding(Configuration* config, const DynamicBitSe& _chromozome);
    BinaryEncoding(Configuration* config, DynamicBitSe&& _chromozome);
    
    void mutate(float mutationChange);
    std::vector<BinaryEncoding> crossover(BinaryEncoding parent2);
    int calculateFittness();
    std::vector<BoxInfo> getBoxPositions();
    Configuration* getConfiguration() const;
private:
    DynamicBitSet generateChromosome(long unsigned int totalBitsNum);
    void repairChromosome();
    
    std::vector<BoxInfo> boxesPositions;
    DynamicBitSet chromozome;
    Configuration* configuration;

};

#endif // BINARYENCODING_H
