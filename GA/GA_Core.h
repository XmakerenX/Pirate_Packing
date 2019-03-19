#pragma once

#include <iostream>
#include <QThread>
#include <cmath>
#include <chrono>
#include <random>
#include <string>

#include "../includes/structs.h"
#include "GA_Random.h"
#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"



template <class Creature>
class GA_Core
{
public:
	static void HybridGeneticAlgorithm(Configuration& configuration, GAThread* callingThread);
	static std::vector<std::vector<BoxInfo>>& getBoxesInfo();

private:
	static void PrintSolution(Creature& c);
	static std::vector<Creature> generateFirstGeneration(Configuration& configuration);
	static void printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration);
	static void getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration);
	static void selectSurvivors(std::vector<Creature>& population);

	static Configuration configuration;
	static int overallMaximumFitness;
	static int generationMaximumFitness;
	static int currentGenPopulationFitness;

	static std::vector<std::vector<BoxInfo>> generationBoxes;
};
