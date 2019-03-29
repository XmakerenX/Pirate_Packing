#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"
#include "GA_Settings.h"

QMutex GAThread::mutex;

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems), stopGeneticAlgorithm(false)
{
}
//----------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, std::vector<Item> givenItems)
	:configuration(containerDimensions, givenItems), stopGeneticAlgorithm(false)
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
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			// apply hybrid genetic algorithm on this configuration
			hybrid.initGeneticAlgorithm(configuration);
			while (hybrid.nextGeneration(configuration))
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
			binary.initGeneticAlgorithm(configuration);
			while (binary.nextGeneration(configuration))
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
	emit GAFinished();
}
//------------------------------------------------------------------------------------------------
void GAThread::resetConfiguration()
{
	configuration.Reset();
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
void GAThread::clearAllGenerations()
{
	switch (GA_Settings::method)
	{
	case GA_Method::HybridGenetic:
	{
		return hybrid.clearAllGenerationsData();
	}
	case GA_Method::PureGenetic:
	{
		return binary.clearAllGenerationsData();
	}
	}
}
