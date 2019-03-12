#ifndef PERMUTATIONENCODING_H
#define PERMUTATIONENCODING_H

#include <vector>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include "../Configuration.h"


class PermutationEncoding 
{
    
public:
    //PermutationEncoding() {};
    PermutationEncoding(Configuration* config);
    void mutate(float mutationChange);
    PermutationEncoding crossover(PermutationEncoding parent2);
    int calculateFittness();
    std::vector<BoxInfo> getBoxPositions();
    Configuration* getConfiguration() const;
    
private:
    std::vector<BoxInfo> boxesPositions;
    std::vector<int> chromozome;
    Configuration* configuration;
    
};

#endif // PERMUTATIONENCODING_H
