#include "BinaryEncoding.h"


BinaryEncoding::BinaryEncoding(Configuration* conf)
    :configuration(conf)
{
    
}

void BinaryEncoding::mutate(float mutationChange)
{
    
}

BinaryEncoding BinaryEncoding::crossover(BinaryEncoding parent2)
{
    return BinaryEncoding(parent2.configuration);
}

int BinaryEncoding::calculateFittness()
{
    return 0;
}

std::vector<BoxInfo> BinaryEncoding::getBoxPositions()
{
    return std::vector<BoxInfo>();
}

Configuration* BinaryEncoding::getConfiguration() const
{
    return this->configuration;
}

