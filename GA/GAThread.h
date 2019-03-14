#ifndef GATHREAD_H
#define GATHREAD_H

#include <iostream>
#include <QThread>
#include <cmath>

#include <chrono>
#include <random>

#include "Configuration.h"
#include "Creature.h"
#include "Breeder.h"
#include "GA_Random.h"
#include "../includes/structs.h"
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
	GAThread(Dimensions containerDimensions, int nItems, int _guiWidth);

	Q_OBJECT
	void run() override;

public:
	const std::vector<std::vector<Rect>>& getRects()
	{
		return generationsRects;
	}

	const std::vector<Rect>& getItemRects()
	{
		return itemsRects;
	}

signals:
	void rectsReady(int index, int overallMaximumFitness, double areaCovered, int currentGenPopulationFitness);
	void itemRectsUpdate(int newHeight);
	void GAStarted();
	void GAFinished();

private:
	/*void PrintSolution(Creature& c);
	std::vector<Creature> generateFirstGeneration(Configuration& configuration);
	void printFinalDataAndSaveResulsts(std::vector<Creature>& population, Configuration& configuration);
	void getDataFromGeneration(std::vector<Creature>& population, Configuration& configuration);
	void selectSurvivors(std::vector<Creature>& population);
	*/
	void HybridGeneticAlgorithm(Configuration& configuration);
	Configuration configuration;
	int overallMaximumFitness;
	int generationMaximumFitness;
	int currentGenPopulationFitness;

	int guiWidth;

	std::vector<std::vector<Rect>> generationsRects;
	std::vector<Rect> itemsRects;

	std::default_random_engine generator;
	const int numberOfGenerations = 80;
	const int populationSize = 100;
};

#endif // GATHREAD_H
