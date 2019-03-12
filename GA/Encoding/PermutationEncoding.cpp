#include "PermutationEncoding.h"



PermutationEncoding::PermutationEncoding(Configuration* conf)
    :configuration(conf)
{
    
}
void PermutationEncoding::mutate(float mutationChange)
{
    
}

PermutationEncoding PermutationEncoding::crossover(PermutationEncoding parent2)
{
    Configuration* conf = this->configuration;
	std::uniform_int_distribution<int> cromozomesIndexes(0, conf->numberOfItems-1);
	int PMX_StartIndex;
	int PMX_EndIndex;

	do { 
		//get the first index
		PMX_StartIndex = cromozomesIndexes(Random::default_engine.getGenerator());
		//get the second unique index
		do
		{
			PMX_EndIndex = cromozomesIndexes(Random::default_engine.getGenerator());
		} while (PMX_EndIndex == PMX_StartIndex);
	} while ((PMX_EndIndex - PMX_StartIndex) <= (conf->numberOfItems/2));//dont allow too big crossing points
    
    
    return PermutationEncoding(this->configuration);
}

int PermutationEncoding::calculateFittness()
{
    return 0;
}

std::vector<BoxInfo> PermutationEncoding::getBoxPositions()
{
    return std::vector<BoxInfo>();
}


Configuration* PermutationEncoding::getConfiguration() const
{
    return this->configuration;
}

