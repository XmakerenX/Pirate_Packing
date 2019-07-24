#pragma once
#include <chrono>
#include <random>

class Random
{
public:
	Random()
	{
		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		seed = currentTime.time_since_epoch().count();
		generator.seed(seed);
	}
    
	Random(unsigned long int _seed)
	{
		seed = _seed;
		generator.seed(seed);
	}
    
	unsigned long int getSeed()
	{
		return seed;
	}
    
	std::default_random_engine& getGenerator()
	{
		return generator;
	}
    
	static Random default_engine;
    
private:
	unsigned long int seed;
	std::default_random_engine generator;
};
