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



void readFile(char* filename);
void validateInput(std::string inputString);
void parseInput(std::string input);

GAThread* GA;
int main(int argc, char** argv)
{
	if ((argc == 2) || (QString(argv[1]) == "-help"))
	{
		std::cout << "\nFormat is: <filename> <method> <populationSize> <numberOfGenerations> <MutationRate> <elitePercentage>\n";
		std::cout << "method parameters:\n -h  = hybrids\n -b  = pure genetics";
		return -1;
	}
	//GA = new GAThread(Dimensions(10,10,10), 100);
	std::cout << "number of arguments given " << argc << "\n";
	
	if (argc < 7)
	{
		std::cout << "not enough arugments given or invalid format";
		return -1;
	}
	
	char* _fileName = argv[1];
	QString method  = argv[2];
	int _populationSize = QString(argv[3]).toInt();
	int _numberOfGenerations = QString(argv[4]).toInt();
	float _mutationRate = QString(argv[5]).toFloat();
	float _elitismPercentage = QString(argv[6]).toFloat();

	std::cout << _fileName << "\n";
	std::cout << method.toStdString()<<"\n";
	std::cout << _populationSize << "\n";
	std::cout << _numberOfGenerations << "\n";
	std::cout << _mutationRate << "\n";
	std::cout << _elitismPercentage << "\n";

	
	GA = new GAThread(Dimensions(10,10,10), 100);
	if (method == "-h")
	{
		GA_Settings::method = GA_Method::HybridGenetic;
	}
	else
	{
		if (method == "-b") {
			GA_Settings::method = GA_Method::PureGenetic;
		}
		else
		{
			std::cout << "Invaid method given\n";
			return -1;
		}
	}
	GA_Settings::populationSize = _populationSize;
	GA_Settings::numberOfGenerations = _numberOfGenerations;
	GA_Settings::elitismSizeGroup = _populationSize * _elitismPercentage;
	GA_Settings::mutationRate = _mutationRate;

	try
	{
		//readFile(_fileName);
		readFile("settings.txt");
	}
	catch (InvalidInputException exeption)
	{
		std::cout << "Error: " << exeption.what();
		return -1;
	}

	std::cout << "\n All data are valid, starting the GA algorithm\n";
	GA->start();
	while(! GA->GeneticAlgorithmFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
	std::cout << "\nSaving results in Config folder...\n";
	GA->saveConfiguration();

	 return 0;
}
//----------------------------------------------------------
void readFile(char* filename)
{
	std::ifstream inFile;
	inFile.open(filename); //open the input file

	std::stringstream strStream;
	strStream << inFile.rdbuf(); //read the file
	std::string input = strStream.str(); //str holds the content of the file

	if (input == "") return;
	std::regex rexp("[^\\d | \\. | \\b | \\n]");//regex to keep only numbers
	std::string input_numbersOnly = std::regex_replace(input, rexp, "");
	
	//validate and parse:
	validateInput(input_numbersOnly);
	parseInput(input_numbersOnly);//this creates a new instance for GA paramter
}
//----------------------------------------------------------
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
//-----------------------------------------------------------------------------------
void parseInput(std::string input)
{
	std::vector<Item> givenItemList;
	int container_width, container_height, container_depth;

	try
	{
		std::stringstream myInputParser(input);
		//parse container:

		myInputParser >> container_width;
		myInputParser >> container_height;
		myInputParser >> container_depth;

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
	GA->setConfigurationData(Dimensions(container_width, container_height,container_depth), std::move(givenItemList));

}
//-------------------------------------------------------------------------------------------