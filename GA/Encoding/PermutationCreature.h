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
	void setSharedFitness(int newSharedFitness);
	int getSharedFitness() const;

	int hammingDistance(PermutationCreature& other);
    bool validateConstraints() {return true;};
    

private:
	void initializeCrossOverPoints(int& startPos, int& endPos);
	void createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
						   Chromozome parent1_chromozome, Chromozome parent2_chromozome);
	int swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap);

	BoxInfo bottomLeftFill(Item item);
	bool isIndexFit(int i, int j,int k, Item item);

    bool*** booleanGraphsSpaces;
	int fitness;
	int sharedFitness;
    std::vector<BoxInfo> boxesPositions;
    std::vector<int> chromozome;
    Configuration* configuration;
    
};

#endif // PERMUTATIONENCODING_H
