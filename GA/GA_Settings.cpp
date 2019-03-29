#include "GA_Settings.h"


GA_Method    GA_Settings::method = GA_Method::PureGenetic;
float        GA_Settings::mutationRate = 0.2;
unsigned int GA_Settings::numberOfGenerations = 200;
int GA_Settings::populationSize = 100;
unsigned int GA_Settings:: elitismSizeGroup = 5;
