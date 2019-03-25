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
#include "GA_Core.h"

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
	GAThread(Dimensions containerDimensions, std::vector<Item> givenItems);
	std::vector<std::vector<BoxInfo>>& getBoxesInfo();  
	void emitBoxReady(int generationBoxesSize);
	void resetConfiguration();

	Q_OBJECT
	void run() override;
    

    
signals:
	void boxesReady(GAThread* ga, int index);
	void GAStarted();
	void GAFinished();

private:
	Configuration configuration;
        GA_Core<PermutationCreature> hybrid;
        GA_Core<BinaryCreature> binary;
};

#endif // GATHREAD_H
