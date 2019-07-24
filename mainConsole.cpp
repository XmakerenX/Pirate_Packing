#include <QApplication>
#include <QtCore>
#include "GA/GA_Settings.h"
#include "GA/Item.h"
#include "GA/GAThread.h"
#include "includes/structs.h"
#include <QFileDialog>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <QDesktopWidget>
#include <QWidget>
#include <QApplication>
#include <iostream>
#include <fstream>
#include <regex>
#include <stack>
#include "GUI/OpenGL/Solutionviewer.h"
#include <sstream>
#include <thread>
#include <chrono>
#include "GraphUtil/gnuplot-iostream.h"
#include <fstream>


int  processArgs(int argc, char** argv, std::vector<GAThread>& threads);
void readFile(const std::string& filename, Dimensions& dim, std::vector<Item>& givenItemList);
void validateInput(std::string inputString);
void parseInput(std::string input, Dimensions& dim, std::vector<Item>& givenItemList);

//-----------------------------------------------------------------------------
// Name   : main
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	
	std::vector<GAThread> threads;
	int argsProcessedOK = processArgs(argc, argv, threads);
	if (argsProcessedOK != 0)
		return argsProcessedOK;
    
	std::cout << "\n All data are valid, starting the GA algorithm\n";
	for (GAThread& GA : threads)
		GA.start();

	for (GAThread& GA : threads)
		GA.wait();
    


	
	//tell the user about the current action
	std::cout << "Saving results in Output folder...\n";
	std::string plotData = "";

	//open log file:
	std::ofstream binaryLogFile;
	std::ofstream hybridLogFile;
	binaryLogFile.open("log_binary.txt", std::ios_base::app);
	hybridLogFile.open("log_hybrid.txt", std::ios_base::app);

	for (GAThread& GA : threads)
	{
		std::string resultFile = GA.saveResults();
		if (resultFile != "")
		{
			std::ofstream* logFile = GA.getSettings().method == GA_Method::HybridGenetic ? &hybridLogFile : &binaryLogFile;
			int finalResult = GA.getGenerationData(GA.getSettings().numberOfGenerations - 1).overallValue;
			*logFile << QString(argv[1]).toStdString()<<" Mutation:" << GA.getSettings().mutationRate << " "<< finalResult << "\n";
			std::cout << "Result save to " << resultFile << "\n";
			plotData = plotData + "'" + resultFile + "' index 'Value' with line," +
                        "'" + resultFile + "'" + " index 'Averge' with line, ";
		}
		else
			std::cout << "Result has failed to save\n";
	}
	binaryLogFile.close();
	hybridLogFile.close();
	if(plotData.empty())
		return -1;
    
	if (plotData != "")
	{
		plotData.pop_back(); // remove the last ','
		if(!QDir("Graph").exists())
			if(!QDir().mkdir(("Graph")))
				return -1;
            
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string timeAndDate(30, '\0');
		std::strftime(&timeAndDate[0], timeAndDate.size(), "%Y-%m-%d_%H-%M-%S.png", std::localtime(&now));
		timeAndDate = timeAndDate.substr(0, timeAndDate.find_first_of('\0'));
        
		Gnuplot gp; 
		gp << "set terminal png linewidth 3 size 1280,960" << std::endl;
		gp << "set key right bottom" << std::endl;
		gp << "set output 'Graph/" << timeAndDate << "'" << std::endl;
		//gp << "set output 'output.png' " << std::endl;
		gp << "plot " <<  plotData << std::endl;
	}
    
	return 0;
}

//-----------------------------------------------------------------------------
// Name   : processArgs 
// Action : process the args passed to main
//-----------------------------------------------------------------------------
int  processArgs(int argc, char** argv, std::vector<GAThread>& threads)
{
	if ((argc == 2) || (QString(argv[1]) == "-help"))
	{
		std::cout << "\nFormat is: <filename> <method> <settings info> <method> <settings info> ... <method> <settings info>\n";
		std::cout << "method parameters:\n -h  = hybrids\n -b  = pure genetics\n";
		std::cout << "Format of settings info : <populationSize> <numberOfGenerations> <MutationRate> <elitePercentage> <-S/-M> <-eN/-dN>\n";

		return -1;
	}
	std::cout << "number of arguments given " << argc << "\n";
	
	if (argc <= 8 || ((argc - 2) % 7 != 0) )
	{
		std::cout << "not enough arugments given or invalid format\n";
		return -1;
	}
	
	Dimensions dim;
	std::vector<Item> items;
	std::string _fileName = argv[1];
	std::cout << _fileName << "\n";
	try
	{
		readFile(_fileName, dim, items);
	}
	catch (InvalidInputException exeption)
	{
		std::cout << "Error: " << exeption.what() << "\n";
		return -1;
	}
    
	for (int i = 0; i < (argc - 2) / 7 ; i++)
	{
		QString methodStr  = argv[2 + i*7];
		std::cout << methodStr.toStdString()<<"\n";
		GA_Method method;
		if (methodStr == "-h")
			method = GA_Method::HybridGenetic;
		else
		{
			if (methodStr == "-b") 
				method = GA_Method::PureGenetic;
			else
			{
				std::cout << "Invaid method given\n";
				return -1;
			}
		}

		int _populationSize = QString(argv[3 + i*7]).toInt();
		int _numberOfGenerations = QString(argv[4 + i*7]).toInt();
		float _mutationRate = QString(argv[5+ i*7]).toFloat();
		float _elitismPercentage = QString(argv[6 + i*7]).toFloat();
                
		bool multiThread = false;
		QString threadMethod(argv[7 + i*7]);
		if (threadMethod == "-S")
			multiThread = false;
		else
			if (threadMethod == "-M")
				multiThread = true;
			else
			{
				std::cout << "Invaid Thread method given\n";
				return -1;                        
			}
			
		bool nitching = false;
		QString nitchingMethod(argv[8 + i*7]);
		if (nitchingMethod == "-eN")
			nitching = true;
		else
			if(nitchingMethod == "-dN")
				nitching = false;
			else
			{
				std::cout << "Invaid nitching parameter given\n";
				return -1;                          
			}
        
		std::cout << _populationSize << "\n";
		std::cout << _numberOfGenerations << "\n";
		std::cout << _mutationRate << "\n";
		std::cout << _elitismPercentage << "\n";
        
		GA_Settings settings(method, _mutationRate, _numberOfGenerations, _populationSize, _populationSize * _elitismPercentage, multiThread, nitching);
        
		threads.emplace_back(dim, items.size(), Random::default_engine.getSeed());
		threads.back().setConfigurationData(dim, items);
		threads.back().setSettings(settings);
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Name   : readFile 
// Action : reads the configuration from file
//-----------------------------------------------------------------------------
void readFile(const std::string& filename, Dimensions& dim, std::vector<Item>& givenItemList)
{
	std::ifstream inFile;
	inFile.open(filename); //open the input file
	if (!inFile.good())
	{
		InvalidInputException ex;
		ex.setErrorMsg("Failed to open file");
		throw ex;
	}

	std::stringstream strStream;
	strStream << inFile.rdbuf(); //read the file
	std::string input = strStream.str(); //str holds the content of the file

	if (input == "") return;
	std::regex rexp("[^\\d | \\. | \\b | \\n]");//regex to keep only numbers
	std::string input_numbersOnly = std::regex_replace(input, rexp, "");
	
	//validate and parse:
	validateInput(input_numbersOnly);
	parseInput(input_numbersOnly, dim, givenItemList);
}

//-----------------------------------------------------------------------------
// Name   : validateInput 
// Action : validate the file read had valid input
//-----------------------------------------------------------------------------
void validateInput(std::string inputString)
{
	if (inputString == "")
	{
		InvalidInputException ex;
		ex.setErrorMsg("input doesnt contain any numeric info");
		throw ex;
	}

	std::stringstream myInputParser(inputString);
	int numberOfIntegresFoundInInput = 0;
	float var;
	while (myInputParser)
	{
		myInputParser >> var;
		numberOfIntegresFoundInInput++;
		if (var - (int)var != 0)
		{
			InvalidInputException ex;
			ex.setErrorMsg("input cant contain a float value!");
			throw ex;
		}
	}
	if ((numberOfIntegresFoundInInput < 7) && (!((numberOfIntegresFoundInInput - 3) % 4 == 0)))
	{
		InvalidInputException ex;
		ex.setErrorMsg("input contains incomplete data");
		throw ex;
	}
}

//-----------------------------------------------------------------------------
// Name   : parseInput 
// Action : process the configuration input data
//-----------------------------------------------------------------------------
void parseInput(std::string input, Dimensions& dim, std::vector<Item>& givenItemList)
{
	try
	{
		std::stringstream myInputParser(input);
		//parse container:
		myInputParser >> dim.w;
		myInputParser >> dim.h;
		myInputParser >> dim.d;

		//parse items
		int id = 0;
		int item_width, item_height, item_depth, item_value;

		while (myInputParser >> item_width)
		{
			//parse a singel item:
			myInputParser >> item_height;
			myInputParser >> item_depth;
			myInputParser >> item_value;
			Item item(Dimensions(item_width, item_height, item_depth), item_value, id);
			givenItemList.emplace_back(item);
			id++;
		}
	}
	catch (std::exception exp)
	{
		std::cout << exp.what();
		throw InvalidInputException();
	}
}
