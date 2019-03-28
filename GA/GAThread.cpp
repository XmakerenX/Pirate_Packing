#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"
#include "GA_Settings.h"
#include "GA_Settings.h"

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems), stopGeneticAlgorithm(false)
{
	allGenerationsData.clear();
	allGenerationsData.reserve(GA_Settings::numberOfGenerations);

}
//----------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, std::vector<Item> givenItems)
	:configuration(containerDimensions, givenItems), stopGeneticAlgorithm(false)
{
	allGenerationsData.clear();
	allGenerationsData.reserve(GA_Settings::numberOfGenerations);

}
//----------------------------------------------------------------------
std::vector<std::vector<BoxInfo>>& GAThread::getBoxesInfo()
{
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			return hybrid.getBoxesInfo();
		}
		case GA_Method::PureGenetic:
		{
			return binary.getBoxesInfo();
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

	int generationNumber = 0;
	allGenerationsData.clear();
	allGenerationsData.reserve(GA_Settings::numberOfGenerations);

	emit GAStarted();
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			// apply hybrid genetic algorithm on this configuration
			hybrid.initGeneticAlgorithm(configuration);
			while (hybrid.nextGeneration(configuration))
			{
				while (this->stopGeneticAlgorithm)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
				}
				emit boxesReady(this, hybrid.getBoxesInfoIndex());
				allGenerationsData.emplace_back(hybrid.currentGenerationData);
				emit generationPassed(generationNumber);
				generationNumber++;
			}
			break;
		}
		case GA_Method::PureGenetic:
		{
			// apply pure genetic algorithm on this configuration
			binary.initGeneticAlgorithm(configuration);
			while (binary.nextGeneration(configuration))
			{
				while (this->stopGeneticAlgorithm)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
				}
				emit boxesReady(this, binary.getBoxesInfoIndex());
				allGenerationsData.emplace_back(binary.currentGenerationData);
				emit generationPassed(generationNumber);
				generationNumber++;
			}
			break;
		}
	}
	emit GAFinished();
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void GAThread::resetConfiguration()
{
	configuration.Reset();
}