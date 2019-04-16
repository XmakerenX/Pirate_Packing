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
	std::vector<BoxInfo>& getBoxesInfo(int index);
	const GenerationData& getGenerationData(int index);
	int getGenerationDataIndex();

	//population related:
	std::vector<Creature> getPopulation();
	void replacePopulation(std::vector<Creature>& newPopulation);
	void saveGenerationData(const std::string& methodPrefix);
        
private:
	std::vector<Creature> generateFirstGeneration(Configuration& configuration);
	void getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration);
	void selectSurvivors(std::vector<Creature>& population);
	
	//Configuration configuration;
	int overallMaximumFitness;
	int generationMaximumFitness;
	
	std::vector<GenerationData> generationData;
	std::vector<Creature> population;
	unsigned int gen;
};
