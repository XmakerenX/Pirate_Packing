#ifndef BINARYENCODING_H
#define BINARYENCODING_H

#include <cmath>
#include <vector>
#include <algorithm>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include <iostream>
#include "../Configuration.h"

typedef boost::dynamic_bitset<> DynamicBitSet;

struct ItemInfo
{
	long unsigned int x;
	long unsigned int y;
	long unsigned int z;
	long unsigned int width;
	long unsigned int height;
	long unsigned int depth;
	int orientaion;
};

struct Box
{
	Box(int _left, int _bottom, int _back, int _right, int _top, int _front)
		:left(_left), bottom(_bottom), back(_back), right(_right), top(_top), front(_front)
	{}
	Box(int _left, int _right, int _bottom, int _top ,int _back, int _front,bool s)
		:left(_left), bottom(_bottom), back(_back), right(_right), top(_top), front(_front)
	{}

	long int getWidth() const
	{
		return right - left;
	}
    
	long int getHeight() const 
	{
		return top - bottom;
	}
    
	long int getDepth() const
	{
		return front - back;
	}
    
	static Box intersect(const Box& a, const Box& b)
	{
		return Box(std::max(a.left, b.left), std::max(a.bottom, b.bottom), std::max(a.back, b.back),
			   std::min(a.right, b.right), std::min(a.top, b.top), std::min(a.front, b.front));
	}
	
	static bool boxConnected(const Box& a, const Box& b)
	{
		Box box = Box::intersect(a, b);
		if (box.getWidth() == 0 && box.getHeight() > 0 && box.getDepth() > 0)		return true;
		if(box.getHeight() == 0 && box.getWidth() > 0 && box.getDepth() > 0)		return true;
		if (box.getDepth() == 0 && box.getWidth() > 0 && box.getHeight() > 0)		return true;
		return false;
	}
	static bool isBoxAtCorner(Configuration* configuration, Box itemBox)
	{
		if ((configuration->dim.w - itemBox.right == 0) ||
			(itemBox.left == 0) ||
			(configuration->dim.h - itemBox.top == 0) ||
			(itemBox.bottom == 0) ||
			(configuration->dim.d - itemBox.front == 0) ||
			(itemBox.back == 0))
		{
			return true;
		}

		return false;
	}

	static int touch(const Box& a, const Box& b)
	{
		APoint box1StartPosition(a.left, a.bottom, a.back );
		APoint box1EndPosition(a.left + (a.right - a.left), a.bottom + (a.top - a.bottom), a.back + (a.front - a.back));
		APoint box2StartPosition(b.left, b.bottom, b.back);
		APoint box2EndPosition(b.left + (b.right - b.left), b.bottom + (b.top - b.bottom), b.back + (b.front - b.back));

	
		int touchArea = 0;
		for (int x = box1StartPosition.x; x <= box1EndPosition.x; x++)
		{
			for (int y = box1StartPosition.y; y <= box1EndPosition.y; y++)
			{
				for (int z = box1StartPosition.z; z <= box1EndPosition.z; z++)
				{
					if (((x <= box2EndPosition.x) && (x >= box2StartPosition.x)) &&
						((y <= box2EndPosition.y) && (y >= box2StartPosition.y)) &&
						((y <= box2EndPosition.z) && (z >= box2StartPosition.z)))
					{
						touchArea++;
					}
				}

			}
		}
		return touchArea;
	}
	
	long  int left;
	long  int bottom;
	long  int back;
	long  int right;
	long  int top;
	long  int front;
};

class BinaryCreature 
{
public:
	BinaryCreature(Configuration* config, Random& randomEngine = Random::default_engine);
	BinaryCreature(Configuration* config, const DynamicBitSet& _chromozome);
	BinaryCreature(Configuration* config, DynamicBitSet&& _chromozome);
	BinaryCreature(const BinaryCreature& copy);
	BinaryCreature(BinaryCreature&& move);
    
	BinaryCreature& operator=(const BinaryCreature& copy);
	BinaryCreature& operator=(BinaryCreature&& move);
    
	void resetChromosome();
	void mutate(float mutationChance, Random& randomEngine = Random::default_engine);
	void crossover(const BinaryCreature& parent2, std::vector<BinaryCreature>& population, Random& randomEngine = Random::default_engine) const;
	int calculateFittness();
	std::vector<BoxInfo> getBoxPositions();
	Configuration* getConfiguration() const;
	bool validateConstraints();
	void setFitness(int newFitness); 
	int getFitness() const;

	void setSharedFitness(int newSharedFitness);
	int getSharedFitness() const;
	unsigned int getMinDist();
	int hammingDistance(BinaryCreature& b);
    
	static int penaltyWeight;

private:
	DynamicBitSet generateChromosome(long unsigned int totalBitsNum, Random& randomEngine);
	void repairChromosome(Random& randomEngine);
	void adjustDimensionsToOrientation(int orientation, long unsigned int& width, long unsigned int& height, long unsigned int& depth);
    
	void onePointCrossover(const BinaryCreature& parent2, std::vector<BinaryCreature>& population, Random& randomEngine) const;
	void uniformCrossover(const BinaryCreature& parent2, std::vector<BinaryCreature>& population, Random& randomEngine) const;
	bool getItemInfo(DynamicBitSet& itemMask, int itemIndex, ItemInfo& itemInfo);
    
	int fitness;
	int sharedFitness;
	std::vector<BoxInfo> boxesPositions;
	DynamicBitSet chromozome;
	DynamicBitSet activeItemsMask;
	Configuration* configuration;
};

#endif // BINARYENCODING_H
