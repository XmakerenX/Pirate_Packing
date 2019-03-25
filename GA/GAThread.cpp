#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"
#include "GA_Settings.h"

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems)
{
	
}
//----------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, std::vector<Item> givenItems)
	:configuration(containerDimensions, givenItems)
{

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
};
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
	configuration.Reset();
	emit GAStarted();
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			// apply hybrid genetic algorithm on this configuration
			hybrid.initGeneticAlgorithm(configuration);
			while (hybrid.nextGeneration(configuration))
				emit boxesReady(this, hybrid.getBoxesInfoIndex());
			break;
		}
		case GA_Method::PureGenetic:
		{
			// apply pure genetic algorithm on this configuration
			binary.initGeneticAlgorithm(configuration);
			while(binary.nextGeneration(configuration))
				emit boxesReady(this, binary.getBoxesInfoIndex());
			break;
		}
	}
	emit GAFinished();
}
//------------------------------------------------------------------------------------------------
