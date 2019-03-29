#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GUI_utils.h"
#include "GA/GA_Settings.h"
#include "includes/structs.h"
#include <QFileDialog>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) 
:	QMainWindow(parent),
	ui(new Ui::MainWindow),
	containerDim(10, 10, 10)
{
	//Init UI:
	ui->setupUi(this);
	setForms();
}
//------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::setForms()
{
	//Form - Main window
	this->setFixedSize(738, 539);
		//Set icon:
		std::string iconPath = GuiUtils::getFullPath("favicon.ico");
		this->setWindowIcon(QIcon(iconPath.c_str()));
		//Set image:
		std::string logoPath = GuiUtils::getFullPath("PiratePackingLogo.png");
		ui->logo_image->setPixmap(QPixmap(logoPath.c_str()));
		ui->logo_image->setScaledContents(true);
	//########################################################
	
	//Form - Settings 
		ui->populationSizeTextBox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->populationSizeTextBox->setMaxLength(5);
		ui->numberOfGenerationTextBox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->numberOfGenerationTextBox->setMaxLength(5);
		ui->elitisimSizeTextBox->setValidator(new QRegExpValidator(QRegExp("[0-9][0-9]"), this));
		ui->mutationRateTextBox->setValidator(new QRegExpValidator(QRegExp("([0][.][0-9][1-9]|1)"), this));
	//########################################################
	
	//Form - Solution 
		viewer = new SolutionViewer(ui->page_2);
		viewer->setGeometry(QRect(600, 250, 350, 300));
		viewer->setContainerDimensions(containerDim);
		// enable anit-aliasing so boxes will look less shit
		QSurfaceFormat format;
		// Force Qt to use OpenGL core profile
		// Qt is silly and uses by defualt the OpenGL Compatibility profile
		// this can cuase issues with some open source drivers
		format.setVersion(3,3); 
		format.setSamples(4);
		viewer->setFormat(format);
		viewer->show();
		
}
//------------------------------------------------------------------------------------
//load data button:
void MainWindow::on_loadDataButton_clicked()
{
	//get input:
	std::string input = readFileFromUser();
	if (input == "") return;
	std::regex rexp("[^\\d | \\. | \\b | \\n]");//regex to keep only numbers
	std::string input_numbersOnly = std::regex_replace(input, rexp, "");
	
	//validate
	try
	{
		validateInput(input_numbersOnly);
		parseInput(input_numbersOnly);//this creates a new instance for GA paramter

		//set settings menu:
		ui->stackedWidget->setCurrentIndex(2);
		ui->populationSizeTextBox->setText(QString::number(GA_Settings::populationSize));
		ui->numberOfGenerationTextBox->setText(QString::number(GA_Settings::numberOfGenerations));
		ui->mutationRateTextBox->setText(QString::number(GA_Settings::mutationRate));
		ui->elitisimSizeTextBox->setText(QString::number(GA_Settings::elitismSizeGroup));
		this->setFixedSize(813, 837);
	}
	catch (InvalidInputException exeption)
	{
		std::cout << exeption.what();//report exception
		QMessageBox messageBox;//create a new messege box
		messageBox.critical(0, "Error", exeption.what());
		messageBox.setFixedSize(500, 200);
		messageBox.show();
		return;
	}	
}
//-----------------------------------------------------------------------------------
std::string MainWindow::readFileFromUser()
{
	//open input file:
	QFile file(QFileDialog::getOpenFileName(this, tr("Open input data"), "/home/", tr("data Files (*.txt)")));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))    return "";
	QString content = file.readAll();
	return content.toStdString();
}
//------------------------------------------------------------------------------------
void MainWindow::validateInput(std::string inputString)
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
void MainWindow::parseInput(string input)
{
	std::vector<Item> givenItemList;

	try
	{
		std::stringstream myInputParser(input);

		//parse container:
		int container_width, container_height, container_depth;

		myInputParser >> container_width;
		myInputParser >> container_height;
		myInputParser >> container_depth;

		this->containerDim.d = container_depth;
		this->containerDim.h = container_height;
		this->containerDim.w = container_width;
		viewer->setContainerDimensions(containerDim);

		//parse items
		int id = 0;
		int item_width, item_height, item_depth, item_value;

		while (myInputParser>> item_width)
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

	GA = new GAThread(containerDim, givenItemList);
	connect(GA, &GAThread::generationPassed, this, &MainWindow::updateGuiDataCorrespondsToNewGeneration);
	connect(GA, &GAThread::boxesReady, viewer, &SolutionViewer::updateSolutionViewer);
	connect(GA, &GAThread::GAStarted, this, &MainWindow::updateGAStarted);
	connect(GA, &GAThread::GAFinished, this, &MainWindow::updateGAFinished);
}
//------------------------------------------------------------------------------------
void MainWindow::on_enterDataButton_clicked()
{
	
}
//------------------------------------------------------------------------------------
void MainWindow::on_wumpusButton_clicked()
{
	std::cout << "Enter data button clicked\n";
	ui->stackedWidget->setCurrentIndex(2);
	ui->populationSizeTextBox->setText(QString::number(GA_Settings::populationSize));
	ui->numberOfGenerationTextBox->setText(QString::number(GA_Settings::numberOfGenerations));
	ui->mutationRateTextBox->setText(QString::number(GA_Settings::mutationRate));
	ui->elitisimSizeTextBox->setText(QString::number(GA_Settings::elitismSizeGroup));
	this->setFixedSize(813, 837);

	GA = new GAThread(containerDim, 100); 
	GA->resetConfiguration();
	connect(GA, &GAThread::generationPassed, this, &MainWindow::updateGuiDataCorrespondsToNewGeneration);
	connect(GA, &GAThread::boxesReady, viewer, &SolutionViewer::updateSolutionViewer);
	connect(GA, &GAThread::GAStarted, this, &MainWindow::updateGAStarted);
	connect(GA, &GAThread::GAFinished, this, &MainWindow::updateGAFinished);
}
//------------------------------------------------------------------------------------
void MainWindow::on_backButton_clicked()
{
	std::cout << "Settings back button clicked\n";
	ui->stackedWidget->setCurrentIndex(0);
	this->setFixedSize(738, 539);
}
//------------------------------------------------------------------------------------
void MainWindow::on_confirmButton_clicked()
{
	std::cout << "Settings confirm button clicked\n";
	QString textBoxesArray[4];
	textBoxesArray[0] = QString(ui->populationSizeTextBox->text());
	textBoxesArray[1] = QString(ui->elitisimSizeTextBox->text());
	textBoxesArray[2] = QString(ui->numberOfGenerationTextBox->text());
	textBoxesArray[3] = QString(ui->mutationRateTextBox->text());
    
	bool validSettings = true;
	for (QString str : textBoxesArray)
	{
		if (str == "") validSettings = false;
	}
	if (validSettings)
	{
		GA_Settings::populationSize = textBoxesArray[0].toInt();
		GA_Settings::elitismSizeGroup = textBoxesArray[1].toInt()*GA_Settings::populationSize / 100;
		GA_Settings::numberOfGenerations = textBoxesArray[2].toInt();
		if (textBoxesArray[3] == "0.") { GA_Settings::mutationRate = 0; }
		else { GA_Settings::mutationRate = textBoxesArray[3].toFloat(); }

		if (ui->radioButton_HybridGenetics->isChecked()) { GA_Settings::method = GA_Method::HybridGenetic; }
		else { GA_Settings::method = GA_Method::PureGenetic;}


		ui->progressBar->setMinimum(0);
		ui->progressBar->setMaximum(GA_Settings::numberOfGenerations - 1);
		ui->progressBar->setMinimum(0);


		ui->stackedWidget->setCurrentIndex(1);
		this->setFixedSize(1000, 900);
	}
	else
	{
		QMessageBox messageBox;
		messageBox.critical(0, "Error", "some parameters are empty!");
		messageBox.setFixedSize(500, 200);
	}

	
}
//------------------------------------------------------------------------------------
void MainWindow::on_startButton_clicked()
{
    std::cout << "start button clicked\n";
	QString startButtonText = ui->startButton->text();

	if (startButtonText == "Start")
	{
		ui->startButton->setText("Stop");
		GA->start();
	}
	else  if (startButtonText == "Stop")
	{
		GA->stopGeneticAlgorithm = true;
		ui->startButton->setText("Continue");
	}
	else  if (startButtonText == "Continue")
	{
		GA->stopGeneticAlgorithm = false;
                GA->continuePressed.wakeAll();
		ui->startButton->setText("Stop");
	}
}
//------------------------------------------------------------------------------------
void MainWindow::on_generationComboBox_currentIndexChanged(QString indexStr)
{
	//int number = indexStr.toInt() - 1;
	//GenerationData chosenGeneration = GA->allGenerationsData[number];

	/*
	ui->AvaregeFittness->setText(QString::number(chosenGeneration.avarageFittness).mid(0, 4));
	ui->BestGenerationalFIttnessTextBox->setText(QString::number(chosenGeneration.bestCreature_Fittness));
	ui->VolumeFilledTextBox->setText(QString::number(chosenGeneration.bestCreature_VolumeFilled).mid(0, 4));
	ui->ValuePercentageTextBox->setText(QString::number(chosenGeneration.bestCreature_ValuePercentage).mid(0, 4));
	//ui->bestFittnessOverallTextBox->setText(QString(std::to_string(data.bestFittnessUntillThisGeneration).c_str()));

	*/
	//GA->emitBoxReady()
}
//------------------------------------------------------------------------------------
void MainWindow::on_resultsBackButton_clicked()
{
	/*
	ui->stackedWidget->setCurrentIndex(2);
	ui->populationSizeTextBox->setText(QString::number(GA_Settings::populationSize));
	ui->numberOfGenerationTextBox->setText(QString::number(GA_Settings::numberOfGenerations));
	ui->mutationRateTextBox->setText(QString::number(GA_Settings::mutationRate));
	ui->elitisimSizeTextBox->setText(QString::number(GA_Settings::elitismSizeGroup));
	this->setFixedSize(813, 837);*/

}
//------------------------------------------------------------------------------------
void MainWindow::on_resultsResetButton_clicked()
{
	GA->resetConfiguration();
}
//------------------------------------------------------------------------------------
void MainWindow::updateGuiDataCorrespondsToNewGeneration(int currentGeneration)
{
	const GenerationData& data = GA->getGenerationData(currentGeneration);
	ui->AvaregeFittness->setText(QString(std::to_string(data.avarageFittness).c_str()).mid(0,4));
	ui->BestGenerationalFIttnessTextBox->setText(QString(std::to_string(data.bestCreatureFittness).c_str()));
	ui->VolumeFilledTextBox->setText(QString(std::to_string(data.bestCreatureVolumeFilled).c_str()).mid(0, 4));
	ui->ValuePercentageTextBox->setText(QString(std::to_string(data.bestCreatureValuePercentage).c_str()).mid(0, 4));
	ui->bestFittnessOverallTextBox->setText(QString(std::to_string(data.bestOverallFittness).c_str()));
	ui->progressBar->setValue(currentGeneration+1);
	ui->generationComboBox->addItem(QString(currentGeneration));
}
//------------------------------------------------------------------------------------
void MainWindow::updateGAStarted()
{
   ui->resultsResetButton->setEnabled(false);
}
//------------------------------------------------------------------------------------
void MainWindow::updateGAFinished()
{
	ui->startButton->setText("Start");
    ui->resultsResetButton->setEnabled(true);
}
//------------------------------------------------------------------------------------
//----------------------------------------------------------------------
void MainWindow::on_radioButton_HybridGenetics_clicked()
{
	ui->radioButton_pureGenetics->setChecked(false);
	ui->radioButton_HybridGenetics->setChecked(true);

}
//----------------------------------------------------------------------
void MainWindow::on_radioButton_pureGenetics_clicked()
{
	ui->radioButton_HybridGenetics->setChecked(false);
	ui->radioButton_pureGenetics->setChecked(true);
}
//---------------------------------------------------------------------

