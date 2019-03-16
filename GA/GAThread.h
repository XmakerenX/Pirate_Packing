#ifndef GATHREAD_H
#define GATHREAD_H

#include <iostream>
#include <QThread>
#include <cmath>
#include <chrono>
#include <random>

#include "../includes/structs.h"
#include "GA_Random.h"
#include "Configuration.h"
#include "Breeder.h"
#include "Encoding/BinaryCreature.h"
#include "Encoding/PermutationCreature.h"
//-----------------------------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------------------------
struct Rect
{
	Rect(int _left, int _top, int _right, int _bottom, const Color& _color)
		:left(_left), top(_top), right(_right), bottom(_bottom), color(_color)
	{}
	
	static Rect intersect(const Rect& a,const Rect& b)
	{
		return Rect(std::max(a.left, b.left), std::max(a.top, b.top), std::min(a.right, b.right), std::min(a.bottom, b.bottom), a.color);
	}
	
	int getWidth() const
	{
		return right - left;
	}
	
	int getHeight() const
	{
		return bottom - top;
	}
	
	int left;
	int top;
	int right;
	int bottom;
	Color color;
};

class GAThread : public QThread
{
public:
	GAThread(Dimensions containerDimensions, int nItems);

    std::vector<std::vector<BoxInfo>>& getBoxesInfo()
    {
        return generationBoxes;
    }
    
	Q_OBJECT
	void run() override;
    

    
signals:
	void boxesReady(GAThread* ga, int index);
	void GAStarted();
	void GAFinished();

private:
	void PrintSolution(PermutationCreature& c);
	std::vector<PermutationCreature> generateFirstGeneration(Configuration& configuration);
	void printFinalDataAndSaveResulsts(std::vector<PermutationCreature>& population, Configuration& configuration);
	void getDataFromGeneration(std::vector<PermutationCreature>& population, Configuration& configuration);
	void selectSurvivors(std::vector<PermutationCreature>& population);

	void HybridGeneticAlgorithm(Configuration& configuration);
	Configuration configuration;
	int overallMaximumFitness;
	int generationMaximumFitness;
	int currentGenPopulationFitness;

    std::vector<std::vector<BoxInfo>> generationBoxes;

	const int numberOfGenerations = 200;
	const int populationSize = 100;
};

#endif // GATHREAD_H
