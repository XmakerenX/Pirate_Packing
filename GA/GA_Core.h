#pragma once

#include <iostream>
#include <QThread>
#include <cmath>
#include <chrono>
#include <random>
#include <string>

#include "../includes/structs.h"
#include "GA_Random.h"
#include "Configuration.h"
#include "Breeder.h"

template <class Creature>
class GA_Core
{
public:
	void initGeneticAlgorithm(Configuration& configuration);
	bool nextGeneration(Configuration& configuration);
	std::vector<std::vector<BoxInfo>>& getBoxesInfo();
	int getBoxesInfoIndex();
	GenerationData currentGenerationData;
        
private:
	void PrintSolution(Creature& c);
	std::vector<Creature> generateFirstGeneration(Configuration& configuration);
	void getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration);
	void selectSurvivors(std::vector<Creature>& population);

	//Configuration configuration;
	int overallMaximumFitness;
	int generationMaximumFitness;
	int currentGenPopulationFitness;
	
	std::vector<std::vector<BoxInfo>> generationBoxes;
	std::vector<Creature> population;
	std::vector<BoxInfo> bestCreatureBoxInfo;
	unsigned int gen;
};
