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
#include <QDesktopWidget>
#include <QWidget>
#include <QApplication>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) 
:	QMainWindow(parent),
	ui(new Ui::MainWindow),
	containerDim(10, 10, 10)
{
	//Init UI:
	ui->setupUi(this);
	setForms();
    
	ui->tableView->setModel(&itemTable);
    
	GA = new GAThread(containerDim, 100); 
	connect(GA, &GAThread::generationPassed, this, &MainWindow::updateGuiDataCorrespondsToNewGeneration);
	connect(GA, &GAThread::boxesReady, viewer, &SolutionViewer::updateSolutionViewer);
	connect(GA, &GAThread::GAStarted, this, &MainWindow::updateGAStarted);
	connect(GA, &GAThread::GAFinished, this, &MainWindow::updateGAFinished);
}
//------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete ui;
	if (GA)
	{
		if (GA->isRunning())
		{
			GA->exitGeneticAlgorithm = true;
			GA->wait();
		}
		delete GA;
	}
}
void MainWindow::setForms()
{
	//Form - Main window
	this->setFixedSizeAndMoveToCenter(738, 539);
	moveToMainMenu();
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
		viewer->setGeometry(QRect(580, 200, 360, 360));
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
		
	//Form - Enter Data 
		ui->containerWidthTextbox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->containerWidthTextbox->setMaxLength(5);
		ui->containerHeightTextbox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->containerHeightTextbox->setMaxLength(5);
		ui->containerDepthTextbox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->containerDepthTextbox->setMaxLength(5);                
}
//------------------------------------------------------------------------------------
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key::Key_Return && event->modifiers() & Qt::ShiftModifier)
	{
		if (ui->tableView->hasFocus())
		{
			itemTable.addNewRow();
			ui->tableView->scrollToBottom();
			return;
		}
	}
    
	if (event->key() == Qt::Key::Key_Delete)
	{
		if (ui->tableView->hasFocus())
		{
			QItemSelectionModel* select;
			select = ui->tableView->selectionModel();
			QModelIndexList selectedRows =  select->selectedRows();
			int minR = std::numeric_limits<int>::max();
			int maxR = 0;
			for (auto& modelIndex : selectedRows)
			{
				minR = std::min(minR, modelIndex.row());
				maxR = std::max(maxR, modelIndex.row());
			}
			itemTable.removeRows(minR, maxR - minR + 1);
			return;
		}
	}
	// Call defualt key handler if no action was taken on the event
	QMainWindow::keyReleaseEvent(event);
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
		Configuration loadedConfig =  parseInput(input_numbersOnly);

		ui->containerWidthTextbox->setText(QString::number(loadedConfig.dim.w));
		ui->containerHeightTextbox->setText(QString::number(loadedConfig.dim.h));
		ui->containerDepthTextbox->setText(QString::number(loadedConfig.dim.d));
		itemTable.setItemsInTable(std::move(loadedConfig.items));

		//set settings menu:
		pageStack.push(0);
        moveToEnterData(false);
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
Configuration MainWindow::parseInput(std::string input)
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

	return Configuration(containerDim, givenItemList);
}
//------------------------------------------------------------------------------------
void MainWindow::on_enterDataButton_clicked()
{
	std::cout << "Enter data button clicked\n";
	pageStack.push(0);
	moveToEnterData();
}
//------------------------------------------------------------------------------------
void MainWindow::on_wumpusButton_clicked()
{
	std::cout << "Wumpus button clicked\n";
	close();
}
//------------------------------------------------------------------------------------
void MainWindow::on_backButton_clicked()
{
	std::cout << "Settings back button clicked\n";
	moveToPreviousPage();
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
		int populationSize = textBoxesArray[0].toInt();
		unsigned int elitismSizeGroup = textBoxesArray[1].toInt();
		unsigned int numberOfGenerations = textBoxesArray[2].toInt();
		float mutationRate;
		if (textBoxesArray[3] == "0.") { mutationRate = 0; }
		else { mutationRate = textBoxesArray[3].toFloat(); }
		GA_Method method;
		if (ui->radioButton_HybridGenetics->isChecked()) { method = GA_Method::HybridGenetic; }
		else { method = GA_Method::PureGenetic;}
        
		GA->setSettings(GA_Settings(method, mutationRate, numberOfGenerations,
		                populationSize, elitismSizeGroup));
		pageStack.push(2);
		ui->progressBar->setValue(0);
		moveToViewer();
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
	//maybe theres another way to refactor this without using enums for the below states?

	if (startButtonText == "Start")
	{
		ui->startButton->setText("Stop");
		ui->resultsResetButton->setEnabled(false);
		ui->generationComboBox->setEnabled(false);
		GA->exitGeneticAlgorithm = false;
		GA->stopGeneticAlgorithm = false;
		GA->start();
	}
	else  if (startButtonText == "Stop")
	{
		GA->stopGeneticAlgorithm = true;

		ui->generationComboBox->setEnabled(true);
		ui->startButton->setText("Continue");
	}
	else  if (startButtonText == "Continue")
	{
		GA->stopGeneticAlgorithm = false;
		ui->generationComboBox->setEnabled(false);
		GA->continuePressed.wakeAll();
		ui->startButton->setText("Stop");
	}
}
//------------------------------------------------------------------------------------
void MainWindow::on_generationComboBox_currentIndexChanged(QString indexStr)
{
	if (ui->generationComboBox->isEnabled()) 
	{
		int number = indexStr.toInt() - 1;
		GenerationData chosenGeneration = GA->getGenerationData(number);
		ui->AvaregeFittness->setText(QString::number(chosenGeneration.avarageFittness));
		ui->BestGenerationalFIttnessTextBox->setText(QString::number(chosenGeneration.bestCreatureFittness));
		ui->VolumeFilledTextBox->setText(QString::number(chosenGeneration.bestCreatureVolumeFilled).mid(0, 4));
		ui->ValuePercentageTextBox->setText(QString::number(chosenGeneration.bestCreatureValuePercentage).mid(0, 4));
		viewer->updateSolutionViewerWithGivenBoxes(chosenGeneration.bestCreatureBoxInfo);
	}
}
//------------------------------------------------------------------------------------
void MainWindow::on_saveConfiguration_clicked()
{
	std::cout << "saveConfiguration was pressed\n";    
	GA->saveConfiguration();
}
//------------------------------------------------------------------------------------
void MainWindow::on_saveResults_clicked()
{
    std::cout << "saveResults_clicked was pressed\n";
    GA->saveResults();
}
//------------------------------------------------------------------------------------
void MainWindow::on_resultsBackButton_clicked()
{
	ui->progressBar->setValue(0);
	ui->AvaregeFittness->setText("");
	ui->BestGenerationalFIttnessTextBox->setText("");
	ui->VolumeFilledTextBox->setText("");
	ui->ValuePercentageTextBox->setText("");
	ui->bestFittnessOverallTextBox->setText("");
	ui->generationComboBox->setEnabled(false);
	ui->generationComboBox->clear();
	ui->startButton->setText("Start");
	ui->resultsResetButton->setEnabled(true);
	ui->generationComboBox->setEnabled(false);
	if (GA->isRunning())
	{
		GA->continuePressed.wakeAll();
		GA->exitGeneticAlgorithm = true;
	}

	moveToPreviousPage();
}
//------------------------------------------------------------------------------------
void MainWindow::on_resultsResetButton_clicked()
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
		Configuration loadedConfig = parseInput(input_numbersOnly);

		viewer->setContainerDimensions(loadedConfig.dim);
		GA->setConfigurationData(loadedConfig.dim, std::move(loadedConfig.items));
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
//------------------------------------------------------------------------------------
void MainWindow::updateGuiDataCorrespondsToNewGeneration(int currentGeneration)
{
	GenerationData data = GA->getGenerationData(currentGeneration);
	ui->AvaregeFittness->setText(QString::number(data.avarageFittness));
	ui->BestGenerationalFIttnessTextBox->setText(QString::number(data.bestCreatureFittness));
	ui->VolumeFilledTextBox->setText(QString::number(data.bestCreatureVolumeFilled).mid(0, 4));
	ui->ValuePercentageTextBox->setText(QString::number(data.bestCreatureValuePercentage).mid(0, 4));
	ui->bestFittnessOverallTextBox->setText(QString::number(data.bestOverallFittness));

	ui->progressBar->setValue(currentGeneration+1);
	ui->generationComboBox->addItem(QString::number(currentGeneration+1));
	ui->generationComboBox->setCurrentIndex(currentGeneration);
}
//------------------------------------------------------------------------------------
void MainWindow::updateGAStarted()
{
	ui->resultsResetButton->setEnabled(false);
	ui->generationComboBox->setEnabled(false);
	ui->saveResults->setEnabled(false);
}
//------------------------------------------------------------------------------------
void MainWindow::updateGAFinished()
{
	ui->startButton->setText("Start");
	ui->generationComboBox->setEnabled(true);
	ui->resultsResetButton->setEnabled(true);
	ui->saveResults->setEnabled(true);
}
//------------------------------------------------------------------------------------
void MainWindow::moveToMainMenu()
{
	ui->stackedWidget->setCurrentIndex(0);
	this->setFixedSizeAndMoveToCenter(738, 539);
}
//------------------------------------------------------------------------------------
void MainWindow::moveToSettings()
{
	ui->stackedWidget->setCurrentIndex(2);
	const GA_Settings& settings = GA->getSettings();
	ui->populationSizeTextBox->setText(QString::number(settings.populationSize));
	ui->numberOfGenerationTextBox->setText(QString::number(settings.numberOfGenerations));
	ui->mutationRateTextBox->setText(QString::number(settings.mutationRate));
	ui->elitisimSizeTextBox->setText(QString::number(settings.elitismSizeGroup));
	this->setFixedSizeAndMoveToCenter(813, 837);        
}
//------------------------------------------------------------------------------------
void MainWindow::moveToEnterData(bool reset/* = true*/)
{
	ui->stackedWidget->setCurrentIndex(3);
	if (reset)
	{
		ui->containerWidthTextbox->setText("10");
		ui->containerHeightTextbox->setText("10");
		ui->containerDepthTextbox->setText("10");
		itemTable.resetTable();
	}
	this->setFixedSizeAndMoveToCenter(813, 837);
}
//------------------------------------------------------------------------------------
void MainWindow::moveToViewer()
{
	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(GA->getSettings().numberOfGenerations - 1);
	ui->progressBar->setMinimum(0);
	ui->saveResults->setEnabled(false);

	viewer->clearAllBoxes();
	ui->stackedWidget->setCurrentIndex(1);
	this->setFixedSizeAndMoveToCenter(1030, 900);
}
//------------------------------------------------------------------------------------
void MainWindow::moveToPreviousPage()
{
	int pageIndex = pageStack.top();
	pageStack.pop();
	switch(pageIndex)
	{
	case 0:
		moveToMainMenu();
		break;
            
	case 1:
		moveToViewer();
		break;
            
	case 2:
		moveToSettings();
		break;
            
	case 3:
		const Dimensions& containerDim = GA->getContainerDimensions();
		ui->containerWidthTextbox->setText(QString::number(containerDim.w));
		ui->containerHeightTextbox->setText(QString::number(containerDim.h));
		ui->containerDepthTextbox->setText(QString::number(containerDim.d));
		itemTable.setItemsInTable(std::move(GA->getConfigurationItems()));
		moveToEnterData(false);
		break;
	}
}
//------------------------------------------------------------------------------------
void MainWindow::on_enterDataConfirmButton_clicked()
{
	std::cout << "Enter Data confirm button clicked\n";
	int containerWidth = ui->containerWidthTextbox->text().toInt();
	int containerHeight = ui->containerHeightTextbox->text().toInt();
	int containerDepth = ui->containerDepthTextbox->text().toInt();
        
	bool isItemsValid;
	std::vector<Item>& itemsData = itemTable.getItemsData(isItemsValid);
        
	if (containerWidth != 0 && containerHeight != 0 && containerDepth != 0 && isItemsValid)
	{
		containerDim = Dimensions(containerWidth, containerHeight, containerDepth);
		viewer->setContainerDimensions(containerDim);
		GA->setConfigurationData(containerDim, std::move(itemsData));

		pageStack.push(3);
		ui->progressBar->setValue(0);
		moveToSettings();           
	}
	else
	{
		QMessageBox messageBox;
		messageBox.critical(0, "Error", "some parameters are empty!");
		messageBox.setFixedSize(500, 200);            
	}
}
//------------------------------------------------------------------------------------
void MainWindow::on_enterDataBackButton_clicked()
{
	std::cout << "Enter Data back button clicked\n";
	moveToPreviousPage();
}
//------------------------------------------------------------------------------------
void MainWindow::on_addRow_clicked()
{
	itemTable.addNewRow();
	ui->tableView->scrollToBottom();
}
//------------------------------------------------------------------------------------
void MainWindow::on_removeRow_clicked()
{
	QItemSelectionModel* select;
	select = ui->tableView->selectionModel();
	QModelIndexList selectedRows = select->selectedRows();
	int minR = std::numeric_limits<int>::max();
	int maxR = 0;
	for (auto& modelIndex : selectedRows)
	{
		minR = std::min(minR, modelIndex.row());
		maxR = std::max(maxR, modelIndex.row());
	}
	itemTable.removeRows(minR, maxR - minR + 1);
}

//------------------------------------------------------------------------------------
void MainWindow::on_radioButton_HybridGenetics_clicked()
{
	ui->radioButton_pureGenetics->setChecked(false);
	ui->radioButton_HybridGenetics->setChecked(true);
}
//------------------------------------------------------------------------------------
void MainWindow::on_radioButton_pureGenetics_clicked()
{
	ui->radioButton_HybridGenetics->setChecked(false);
	ui->radioButton_pureGenetics->setChecked(true);
}
//------------------------------------------------------------------------------------
void MainWindow::setFixedSizeAndMoveToCenter(int windowWidth, int windowHeight)
{
	this->setFixedSize(windowWidth, windowHeight);

	#ifdef _WIN32
		int x, y;
		int screenWidth, screenHeight;
		QDesktopWidget *desktop = QApplication::desktop();

		screenWidth = desktop->width();
		screenHeight = desktop->height();

		x = (screenWidth - windowWidth) / 2;
		y = (screenHeight - windowHeight) / 2;

		this->move(x, y);
	#endif
}
