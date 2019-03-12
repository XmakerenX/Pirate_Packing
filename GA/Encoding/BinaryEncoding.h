#ifndef BINARYENCODING_H
#define BINARYENCODING_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "../../includes/structs.h"
#include "../Configuration.h"

class BinaryEncoding 
{
public:
    BinaryEncoding(Configuration* config);
    
    void mutate(float mutationChange);
    BinaryEncoding crossover(BinaryEncoding parent2);
    int calculateFittness();
    std::vector<BoxInfo> getBoxPositions();
    Configuration* getConfiguration() const;
private:
    std::vector<BoxInfo> boxesPositions;
    boost::dynamic_bitset<> chromozome;
    Configuration* configuration;

};

#endif // BINARYENCODING_H
