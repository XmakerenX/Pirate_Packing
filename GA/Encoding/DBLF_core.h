#ifndef PERMUTATIONENCODING_H
#define PERMUTATIONENCODING_H

#include <vector>
#include <unordered_map>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include "../Configuration.h"

class DBLF_core
{

public:
	DBLF_core(Configuration* config);
	DBLF_core(Configuration* conf, Chromozome chrom,int numberOfItems);

	void mutate(float mutationChance);
	void crossover(DBLF_core parent2, std::vector<DBLF_core>& population);
	int calculateFittness();
	std::vector<BoxInfoWithID> getBoxPositions();
	Configuration* getConfiguration() const;
	void setFitness(int newFitness);
	int getFitness() const;

	bool validateConstraints() { return true; };


private:
	void initializeCrossOverPoints(int& startPos, int& endPos);
	void createTwoChildren(Chromozome& child1, Chromozome& child2, int min, int max,
		Chromozome parent1_chromozome, Chromozome parent2_chromozome);
	int swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap);

	BoxInfoWithID bottomLeftFill(Item item);
	bool isIndexFit(int i, int j, int k, Item item);

	bool*** booleanGraphsSpaces;
	int fitness = 0;
	std::vector<BoxInfoWithID> boxesPositions;
	std::vector<int> chromozome;
	Configuration* configuration;
	int numberOfItemsToPack;
};

#endif // PERMUTATIONENCODING_H
