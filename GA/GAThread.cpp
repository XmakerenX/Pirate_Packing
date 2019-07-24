#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"

QMutex GAThread::mutex;

//--------------------------------------------------------------------------------------------------
// Name:   GAThread(constructor)
// Input : containerDimensions -  the width,height and depth of the container
//         nItems - the number of items to be the configuration
// Output: a new GAThread
// Action: Creates a new GAThread
//--------------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems, randomEngine),
	 settings(GA_Method::PureGenetic, 0.2, 200, 100, 5),
	 hybrid(randomEngine),
	 binary(randomEngine),
	 stopGeneticAlgorithm(false),
	 exitGeneticAlgorithm(false)
{
}

//--------------------------------------------------------------------------------------------------
// Name:   GAThread(constructor)
// Input : containerDimensions -  the width,height and depth of the container
//         nItems - the number of items to be the configuration
//         seed - the value to seed the random generator of the GAThread
// Output: a new GAThread
// Action: Creates a new GAThread with a randomEngine seeded with the given value
//--------------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems, unsigned long int seed)
	:randomEngine(seed),
	 configuration(containerDimensions, nItems, randomEngine),
	 settings(GA_Method::PureGenetic, 0.2, 200, 100, 5),
	 hybrid(randomEngine),
	 binary(randomEngine),
	 stopGeneticAlgorithm(false),
	 exitGeneticAlgorithm(false)
{}

//--------------------------------------------------------------------------------------------------
// Name:   GAThread(constructor)
// Input : containerDimensions -  the width,height and depth of the container
//         givenItems - the items for the configuration
// Output: a new GAThread
// Action: Creates a new GAThread with configuration with the given items
//--------------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, std::vector<Item> givenItems)
	:configuration(containerDimensions, givenItems),
	 settings(GA_Method::PureGenetic, 0.2, 200, 100, 5),
	 hybrid(randomEngine),
	 binary(randomEngine),
	 stopGeneticAlgorithm(false),
	 exitGeneticAlgorithm(false)
{
}

//--------------------------------------------------------------------------------------------------
// Name:   GAThread(copy constructor)
// Input:  GAThread to be copied
// Output: A copy of the given GAThread
// Action: copy constructor for GAThread
//--------------------------------------------------------------------------------------------------
GAThread::GAThread(const GAThread& copy)
	 :configuration(copy.configuration),
	  settings(copy.settings),
	  randomEngine(copy.randomEngine),
	  hybrid(randomEngine),
	  binary(randomEngine),
	  stopGeneticAlgorithm(copy.stopGeneticAlgorithm),
	  exitGeneticAlgorithm(copy.exitGeneticAlgorithm)
{
}

//--------------------------------------------------------------------------------------------------
// Name:   GAThread(move constructor)
// Input:  GAThread to be moved
// Output: A move of the given GAThread
// Action: move constructor for GAThread
//--------------------------------------------------------------------------------------------------
GAThread::GAThread(GAThread&& move)
	 :configuration(std::move(move.configuration)),
	  randomEngine(std::move(move.randomEngine)),
	  settings(move.settings),
	  hybrid(randomEngine),
	  binary(randomEngine),
	  stopGeneticAlgorithm(move.stopGeneticAlgorithm),
	  exitGeneticAlgorithm(move.exitGeneticAlgorithm)
{
}

//--------------------------------------------------------------------------------------------------
// Name:   getBoxesInfo
// Input:  index - the generation to get boxes info data from
// Output: vector with the boxes info of the requested generation
// Action: returns the boxes info of the generation requested
//--------------------------------------------------------------------------------------------------
std::vector<BoxInfo>& GAThread::getBoxesInfo(int index)
{
	switch (settings.method)
	{
		case GA_Method::HybridGenetic:
		{
			return hybrid.getBoxesInfo(index);
		}
		case GA_Method::PureGenetic:
		{
			return binary.getBoxesInfo(index);
		}
	}
}

//--------------------------------------------------------------------------------------------------
// Name:   emitBoxReady
//--------------------------------------------------------------------------------------------------
void GAThread::emitBoxReady(int generationBoxesSize)
{
	emit boxesReady(this, generationBoxesSize);
}

//--------------------------------------------------------------------------------------------------
// Name:   run
// Action: executes the genetic algorithm , this function is called when this thread is started
//--------------------------------------------------------------------------------------------------
void GAThread::run()
{
	std::cout << "seed " << randomEngine.getSeed() << "\n";
        
	emit GAStarted();
	std::clock_t start; double duration; start = std::clock();
	auto timeStart = std::chrono::high_resolution_clock::now();
	switch (settings.method)
	{
		case GA_Method::HybridGenetic:
		{
			// apply hybrid genetic algorithm on this configuration                        
			hybrid.initGeneticAlgorithm(configuration, settings);
			while (hybrid.nextGeneration(configuration, settings) && !exitGeneticAlgorithm)
			{
				if (this->stopGeneticAlgorithm)
				{
					mutex.lock();
					           continuePressed.wait(&mutex);
					mutex.unlock();
				}
				emit boxesReady(this, hybrid.getGenerationDataIndex());
				emit generationPassed(hybrid.getGenerationDataIndex());
			}
			break;
		}
		case GA_Method::PureGenetic:
		{
			// apply pure genetic algorithm on this configuration
			binary.initGeneticAlgorithm(configuration, settings);
			while (binary.nextGeneration(configuration, settings) && !exitGeneticAlgorithm)
			{
				if (this->stopGeneticAlgorithm)
				{
					mutex.lock();
					           continuePressed.wait(&mutex);
					mutex.unlock();
				}

				emit boxesReady(this, binary.getGenerationDataIndex());
				emit generationPassed(binary.getGenerationDataIndex());
			}
			break;
		}
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::chrono::duration<float>  timeDuration = std::chrono::high_resolution_clock::now() - timeStart;
	std::cout << "Genetic algorithm finished after : " << timeDuration.count() << "\n";
	emit GAFinished();
}

//--------------------------------------------------------------------------------------------------
// Name:   resetConfiguration
//--------------------------------------------------------------------------------------------------
void GAThread::resetConfiguration()
{
	configuration.Reset(randomEngine);
}

//--------------------------------------------------------------------------------------------------
// Name:   saveConfiguration
//--------------------------------------------------------------------------------------------------
void GAThread::saveConfiguration()
{
    configuration.saveToFile();
}

//--------------------------------------------------------------------------------------------------
// Name:   saveResults
// Action: saves the result of the GA algorithm to file
//--------------------------------------------------------------------------------------------------
std::string GAThread::saveResults()
{
	switch (settings.method)
	{
		case GA_Method::HybridGenetic:
		{
			return hybrid.saveGenerationData("Hybrid", settings);
		}
		case GA_Method::PureGenetic:
		{
			return binary.saveGenerationData("Binary", settings);
		}
	}
}

//--------------------------------------------------------------------------------------------------
// Name:   getGenerationData
// Input:  index - the generation to get boxes generation data from
// Output: the generation data of the requested generation
// Action: returns the generation data of the requested generation
//--------------------------------------------------------------------------------------------------
const GenerationData& GAThread::getGenerationData(int index)
{
	switch (settings.method)
	{
		case GA_Method::HybridGenetic:
		{
			return hybrid.getGenerationData(index);
		}
		case GA_Method::PureGenetic:
		{
			return binary.getGenerationData(index);
		}
		default: throw  std::invalid_argument("GA_Method is neither Binary nor Hybrid, and thus an overflow must have happened.");

	}
}

//--------------------------------------------------------------------------------------------------
// Name:   setConfigurationData
//--------------------------------------------------------------------------------------------------
void GAThread::setConfigurationData(const Dimensions& containerDimensions, std::vector<Item>& givenItems)
{
    configuration = Configuration(containerDimensions, givenItems);
}

//--------------------------------------------------------------------------------------------------
// Name:   setConfigurationData
//--------------------------------------------------------------------------------------------------
void GAThread::setConfigurationData(const Dimensions& containerDimensions, std::vector<Item>&& givenItems)
{
    configuration = Configuration(containerDimensions, std::move(givenItems));
}

//--------------------------------------------------------------------------------------------------
// Name:   setSettings
//--------------------------------------------------------------------------------------------------
void GAThread::setSettings(const GA_Settings& _settings)
{
    settings = _settings;
}

//--------------------------------------------------------------------------------------------------
// Name:   getConfigurationItems
//--------------------------------------------------------------------------------------------------
std::vector<Item>& GAThread::getConfigurationItems()
{
	return configuration.items;
}

//--------------------------------------------------------------------------------------------------
// Name:   getContainerDimensions
//--------------------------------------------------------------------------------------------------
const Dimensions& GAThread::getContainerDimensions() const
{
	return configuration.dim;
}

//--------------------------------------------------------------------------------------------------
// Name:   getSettings
//--------------------------------------------------------------------------------------------------
const GA_Settings& GAThread::getSettings() const
{
    return settings;
}
