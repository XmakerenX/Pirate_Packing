#pragma once
#include <unordered_map>
#include "Creature.h"
template <class encoding>
class Breeder
{
	
private: 
	static void mateParents(Creature<encoding>& parent1, Creature<encoding>& parent2, std::vector<Creature<encoding>>& population);
	static void createChildren(Creature<encoding>& parent1, Creature<encoding>& parent2, int PMX_StartIndex, int PMX_EndIndex, std::vector<Creature<encoding>>& population);
	static void mutateChild(Chromozome& chromozome);
	static int  swapRepetition(std::unordered_map<int, int>& hash, int valueToSwap);

public:
	static std::vector<Creature<encoding>> generateNextGeneration(std::vector<Creature<encoding>>&currentPopulation);
	
};
