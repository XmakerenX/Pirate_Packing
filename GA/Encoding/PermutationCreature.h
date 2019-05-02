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
    PermutationCreature(Configuration* config, Random& randomEngine = Random::default_engine);
    PermutationCreature(Configuration* conf, Chromozome chrom);
    PermutationCreature(const PermutationCreature& copy);
    PermutationCreature(PermutationCreature&& move);
    
    PermutationCreature& operator=(const PermutationCreature& copy);
    PermutationCreature& operator=(PermutationCreature&& move);
    
	void mutate(float mutationChance, Random& randomEngine = Random::default_engine);
	void crossover(const PermutationCreature& parent2, std::vector<PermutationCreature>& population, Random& randomEngine = Random::default_engine) const;
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
	void initializeCrossOverPoints(int& startPos, int& endPos, Random& randomEngine) const;
	void createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
						   Chromozome parent1_chromozome, Chromozome parent2_chromozome) const;
	int swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap) const;

	BoxInfo bottomLeftFill(Item item);
	bool isIndexFit(int i, int j,int k, Item item);

	    bool*** booleanGraphsSpaces;
	int fitness;
	int sharedFitness;
	std::vector<BoxInfo> boxesPositions;
	std::vector<int> chromozome;
	std::vector<int> activeItems;
	Configuration* configuration;
    
};

#endif // PERMUTATIONENCODING_H
