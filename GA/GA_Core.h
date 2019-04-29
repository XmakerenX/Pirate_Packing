#pragma once

#include <iostream>
#include <QThread>
#include <cmath>
#include <chrono>
#include <random>
#include <string>

#include "../includes/structs.h"
#include "GA_Random.h"
#include "GA_Settings.h"
#include "Configuration.h"
#include "Breeder.h"

template <class Creature>
class GA_Core
{
public:
	GA_Core(Random& _randomEngine);
	void initGeneticAlgorithm(Configuration& configuration, const GA_Settings& settings);
	bool nextGeneration(Configuration& configuration, const GA_Settings& settings);
	std::vector<BoxInfo>& getBoxesInfo(int index);
	const GenerationData& getGenerationData(int index);
	int getGenerationDataIndex();

	//population related:
	std::vector<Creature> getPopulation();
	void replacePopulation(std::vector<Creature>& newPopulation);
	std::string saveGenerationData(const std::string& methodPrefix,const GA_Settings& settings);
        
private:
	std::vector<Creature> generateFirstGeneration(Configuration& configuration, const GA_Settings& settings);
	void getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration, const GA_Settings& settings);
	void selectSurvivors(std::vector<Creature>& population, const GA_Settings& settings);
	
	Random& randomEngine;
        
	int overallMaximumFitness;
	int generationMaximumFitness;
	
	std::vector<GenerationData> generationData;
	std::vector<Creature> population;
	unsigned int gen;
};
