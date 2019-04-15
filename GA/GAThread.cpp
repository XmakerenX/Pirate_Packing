#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"
#include "GA_Settings.h"

QMutex GAThread::mutex;

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems), stopGeneticAlgorithm(false), exitGeneticAlgorithm(false),GeneticAlgorithmFinished(false)
{
}
//----------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, std::vector<Item> givenItems)
	:configuration(containerDimensions, givenItems), stopGeneticAlgorithm(false), exitGeneticAlgorithm(false), GeneticAlgorithmFinished(false)
{
}
//----------------------------------------------------------------------
std::vector<BoxInfo>& GAThread::getBoxesInfo(int index)
{
	switch (GA_Settings::method)
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
	std::cout << "seed " << Random::default_engine.getSeed() << "\n";
        
	emit GAStarted();
	GeneticAlgorithmFinished = false;
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			// apply hybrid genetic algorithm on this configuration
			hybrid.initGeneticAlgorithm(configuration);
			while (hybrid.nextGeneration(configuration) && !exitGeneticAlgorithm)
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
			int numberOfPopulations = 1; 
			//-------create multiple populations-----------//
			std::vector<GA_Core<BinaryCreature>> binaryPopulations;
			binaryPopulations.push_back(binary);
			for (int pop = 0; pop < numberOfPopulations-1; pop++)
			{
				binaryPopulations.push_back(GA_Core<BinaryCreature>());
			}
			//--------init all populations----------------//
			for (GA_Core<BinaryCreature>& population : binaryPopulations)
			{
				population.initGeneticAlgorithm(configuration);
			}
			binary.initGeneticAlgorithm(configuration);

			//assign a counter to know how many generations have progressed
			int generationsPassed = 0;
			
			// apply pure genetic algorithm on this configuration
			while (binary.nextGeneration(configuration) && !exitGeneticAlgorithm)
			{
				if (numberOfPopulations > 1){
					for (int i = 1; i <numberOfPopulations;i++ )
					{
						binaryPopulations[i].nextGeneration(configuration);
					}
				}
				generationsPassed++;

				//combine multiple populations into one once every 50 generations
				if (  (numberOfPopulations>1) && ((generationsPassed % 50) == 0 )&&
					  (generationsPassed !=0))
				{
					std::vector<BinaryCreature> newPopulation;
					newPopulation.reserve(GA_Settings::populationSize);

					for (GA_Core<BinaryCreature>& population : binaryPopulations)
					{
						for (int i = 0; i < GA_Settings::populationSize / numberOfPopulations; i++)
							newPopulation.push_back(population.getPopulation()[i]);
					}
					while (newPopulation.size() < GA_Settings::populationSize)
					{
						newPopulation.push_back(binary.getPopulation()[GA_Settings::populationSize / 2]);
					}
					//replace old population with the combined one
					binary.replacePopulation(newPopulation);
				}
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
	GeneticAlgorithmFinished = true;
	emit GAFinished();
}
//------------------------------------------------------------------------------------------------
void GAThread::resetConfiguration()
{
	configuration.Reset();
}
//------------------------------------------------------------------------------------------------
void GAThread::saveConfiguration()
{
    configuration.saveToFile();
}
//------------------------------------------------------------------------------------------------
void GAThread::setConfigurationData(Dimensions& containerDimensions, std::vector<Item>& givenItems)
{
    configuration = Configuration(containerDimensions, givenItems);
}
//------------------------------------------------------------------------------------------------
void GAThread::setConfigurationData(Dimensions& containerDimensions, std::vector<Item>&& givenItems)
{
    configuration = Configuration(containerDimensions, std::move(givenItems));
}
//------------------------------------------------------------------------------------------------
const GenerationData& GAThread::getGenerationData(int index)
{
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			return hybrid.getGenerationData(index);
		}
		case GA_Method::PureGenetic:
		{
			return binary.getGenerationData(index);
		}
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
