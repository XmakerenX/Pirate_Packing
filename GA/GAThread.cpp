#include "GAThread.h"
#include "Configuration.h"
#include "Breeder.h"
#include "GA_Random.h"
#include "GA_Settings.h"
#include "GA_Core.h"

//------------------------------------------------------------------------------------------
GAThread::GAThread(Dimensions containerDimensions, int nItems)
	:configuration(containerDimensions, nItems)
{

}
//----------------------------------------------------------------------
 std::vector<std::vector<BoxInfo>>& GAThread::getBoxesInfo()
{
	switch (GA_Settings::method)
	{
		case GA_Method::HybridGenetic:
		{
			return GA_Core<PermutationCreature>::getBoxesInfo();
			break;
		}
	case GA_Method::PureGenetic:
		{
			return GA_Core<BinaryCreature>::getBoxesInfo();
			break;
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
			GA_Core<PermutationCreature>::HybridGeneticAlgorithm(configuration,this);     //apply genetic algorithm on this configuration
			break;
		}
		case GA_Method::PureGenetic:
		{
			GA_Core<BinaryCreature>::HybridGeneticAlgorithm(configuration, this);     //apply genetic algorithm on this configuration
			break;
		}
	defualt:
		{
			break;
		}
	}
	emit GAFinished();
}
//------------------------------------------------------------------------------------------------