#ifndef PERMUTATIONENCODING_H
#define PERMUTATIONENCODING_H

#include <vector>
#include <unordered_map>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include "../Configuration.h"


class PermutationEncoding 
{
    
public:
    PermutationEncoding(Configuration* config);
    void mutate(float mutationChange);
	std::vector<PermutationEncoding> crossover(PermutationEncoding parent2);
    int calculateFittness();
    std::vector<BoxInfo> getBoxPositions();
    Configuration* getConfiguration() const;
    
private:
	void PermutationEncoding::initializeCrossOverPoints(int& startPos, int& endPos);
	PermutationEncoding::PermutationEncoding(Configuration* conf, Chromozome chrom);
	void createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
						   Chromozome parent1_chromozome, Chromozome parent2_chromozome);
	int swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap);
    std::vector<BoxInfo> boxesPositions;
    std::vector<int> chromozome;
    Configuration* configuration;
    
};

#endif // PERMUTATIONENCODING_H
