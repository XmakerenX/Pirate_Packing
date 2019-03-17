#ifndef PERMUTATIONENCODING_H
#define PERMUTATIONENCODING_H

#include <vector>
#include <unordered_map>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include "../Configuration.h"

class PermutationCreature 
{
    
public:
    PermutationCreature(Configuration* config);
    PermutationCreature(Configuration* conf, Chromozome chrom);
    
	void mutate(float mutationChance);
	void crossover(PermutationCreature parent2, std::vector<PermutationCreature>& population);
    int calculateFittness();
    std::vector<BoxInfo> getBoxPositions();
    Configuration* getConfiguration() const;
    void setFitness(int newFitness);
    int getFitness() const;
    

private:
	void initializeCrossOverPoints(int& startPos, int& endPos);
	void createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
						   Chromozome parent1_chromozome, Chromozome parent2_chromozome);
	int swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap);

	BoxInfo bottomLeftFill(Item item);
	bool isIndexFit(int i, int j,int k, Item item);

    int fitness = 0;
    std::vector<BoxInfo> boxesPositions;
    std::vector<int> chromozome;
    Configuration* configuration;
    
};

#endif // PERMUTATIONENCODING_H
