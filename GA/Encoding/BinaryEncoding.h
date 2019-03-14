#ifndef BINARYENCODING_H
#define BINARYENCODING_H

#include <cmath>
#include <vector>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include "../../includes/structs.h"
#include "../GA_Random.h"
#include "../Configuration.h"

typedef boost::dynamic_bitset<> DynamicBitSet;

struct Box
{
    Box(int _left, int _bottom, int _back, int _right, int _top, int _front)
        :left(_left), bottom(_bottom), back(_back), right(_right), top(_top), front(_front)
    {}


    long unsigned int getWidth()
    {
        return right - left;
    }
    
    long unsigned int getHeight()
    {
        return top - bottom;
    }
    
    long unsigned int getDepth()
    {
        return front - back;
    }
    
    static Box intersect(const Box& a, const Box& b)
    {
        return Box(std::max(a.left, b.left), std::max(a.bottom, b.bottom), std::max(a.back, b.back),
                   std::min(a.right, b.right), std::min(a.top, b.top), std::min(a.front, b.front));
    }
    
    long unsigned int left;
    long unsigned int bottom;
    long unsigned int back;
    long unsigned int right;
    long unsigned int top;
    long unsigned int front;
};

class BinaryEncoding 
{
public:
    BinaryEncoding(Configuration* config);
    BinaryEncoding(Configuration* config, const DynamicBitSet& _chromozome);
    BinaryEncoding(Configuration* config, DynamicBitSet&& _chromozome);
    
    void mutate(float mutationChange);
    std::vector<BinaryEncoding> crossover(BinaryEncoding parent2);
    int calculateFittness();
    std::vector<BoxInfo> getBoxPositions();
    Configuration* getConfiguration() const;
private:
    DynamicBitSet generateChromosome(long unsigned int totalBitsNum);
    void repairChromosome();
    void adjustDimensionsToOrientation(int orientation, long unsigned int& width, long unsigned int& height, long unsigned int& depth);
    
    std::vector<BoxInfo> boxesPositions;
    DynamicBitSet chromozome;
    Configuration* configuration;

};

#endif // BINARYENCODING_H
