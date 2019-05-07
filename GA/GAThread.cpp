#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"

QMutex GAThread::mutex;

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems, randomEngine),
	 settings(GA_Method::PureGenetic, 0.2, 200, 100, 5),
	 hybrid(randomEngine),
	 binary(randomEngine),
	 stopGeneticAlgorithm(false),
	 exitGeneticAlgorithm(false)
{
}
//----------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems, unsigned long int seed)
	:randomEngine(seed),
	 configuration(containerDimensions, nItems, randomEngine),
	 settings(GA_Method::PureGenetic, 0.2, 200, 100, 5),
	 hybrid(randomEngine),
	 binary(randomEngine),
	 stopGeneticAlgorithm(false),
	 exitGeneticAlgorithm(false)
{}
//----------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, std::vector<Item> givenItems)
	:configuration(containerDimensions, givenItems),
	 settings(GA_Method::PureGenetic, 0.2, 200, 100, 5),
	 hybrid(randomEngine),
	 binary(randomEngine),
	 stopGeneticAlgorithm(false),
	 exitGeneticAlgorithm(false)
{
}
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
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
//---------------------------------------------------------------------
void GAThread::emitBoxReady(int generationBoxesSize)
{
	emit boxesReady(this, generationBoxesSize);
}

//-----------------------------------------------------------------------------------------------
// run
//-----------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------
void GAThread::resetConfiguration()
{
	configuration.Reset(randomEngine);
}
//------------------------------------------------------------------------------------------------
void GAThread::saveConfiguration()
{
    configuration.saveToFile();
}
//------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------
void GAThread::setConfigurationData(const Dimensions& containerDimensions, std::vector<Item>& givenItems)
{
    configuration = Configuration(containerDimensions, givenItems);
}
//------------------------------------------------------------------------------------------------
void GAThread::setConfigurationData(const Dimensions& containerDimensions, std::vector<Item>&& givenItems)
{
    configuration = Configuration(containerDimensions, std::move(givenItems));
}
//------------------------------------------------------------------------------------------------
void GAThread::setSettings(const GA_Settings& _settings)
{
    settings = _settings;
}
//------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------
std::vector<Item>& GAThread::getConfigurationItems()
{
	return configuration.items;
}
//------------------------------------------------------------------------------------------------
const Dimensions& GAThread::getContainerDimensions() const
{
	return configuration.dim;
}
//------------------------------------------------------------------------------------------------
const GA_Settings& GAThread::getSettings() const
{
    return settings;
}
