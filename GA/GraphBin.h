#pragma once
#include "../includes/structs.h"
#include "Configuration.h"

class GraphBin
{
private:
	Position bottomLeftFill(Item item);
	bool isIndexFit(int i, int j, Item item);
	void initAndClearTheGraph();
	void freeTheGraph();
public:
	GraphBin(const Chromozome& chromozome,Configuration* conf);
	GraphBin(const GraphBin& g);
	GraphBin(GraphBin&& move);
	~GraphBin();
            
	GraphBin& operator=(const GraphBin& g);
	GraphBin& operator=(GraphBin&& g);
        
	Configuration* configuration;
	Chromozome chromozome;
	int cellsFilled = 0;
	std::vector<Position> itemsPosition;
	bool**   booleanGraphsSpaces;//We represent the  bin as a 2D graph, with each node taking a 1*1 size
								 //So, for example, a graph with width = 150 and height = 300 will be composed 
								 //as a 300 rows of 150 coloums in each row. 
};



