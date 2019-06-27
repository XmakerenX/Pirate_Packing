#include "viewerOnlyWindow.h"
#include "ui_viewerOnlyWindow.h"
#include "includes/structs.h"
#include <QFileDialog>
#include <QString>
#include <QWidget>
#include <QApplication>
#include <iostream>
#include <fstream>

//--------------------------------------------------------------------------------------------------
// Name  : ViewerOnlyWindow(constructor)
// Input : parent - the parent widget of this window
// Action: Creates a  ItemTableModel
//--------------------------------------------------------------------------------------------------
ViewerOnlyWindow::ViewerOnlyWindow(QWidget *parent) 
:	QMainWindow(parent),
	ui(new Ui::ViewerOnlyWindow),
	containerDim(10, 10, 10)
{
	//Init UI:
	ui->setupUi(this);
	setForms();
}

//-----------------------------------------------------------------------------
// Name   : ViewerOnlyWindow (Destructor)
// Action : destroys the ViewerOnlyWindow and frees the memory it allocated
//-----------------------------------------------------------------------------
ViewerOnlyWindow::~ViewerOnlyWindow()
{
	delete ui;
}

//--------------------------------------------------------------------------------------------------
// Name:   setForms
// Action: setup the form widgets
//--------------------------------------------------------------------------------------------------
void ViewerOnlyWindow::setForms()
{
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
}

//--------------------------------------------------------------------------------------------------
// Name:   on_loadFileButton_clicked
// Action: load a result file and display it's results
//--------------------------------------------------------------------------------------------------
void ViewerOnlyWindow::on_loadFileButton_clicked()
{
	std::cout << "load file button clicked\n";
	std::string inputFilePath = QFileDialog::getOpenFileName(this, tr("Open input data"), "/home/", tr("data Files (*.dat)")).toStdString();
	if (inputFilePath == "")
	{
		std::cout << "user aborted file selection\n";
		return;
	}
	std::ifstream inputFile (inputFilePath);
	std::string lineData;
	do
	{
		std::getline(inputFile, lineData);
	}while(lineData != "#finalBoxPosition" && !inputFile.eof() && !inputFile.fail());
    
	if (inputFile.eof() || inputFile.fail())
	{
		std::cout << "No boxes Position section was found in file, Aborting!!\n";
		return;
	}
    
	std::vector<BoxInfo> boxesPositions;
	while(!inputFile.eof() && !inputFile.fail())
	{
		boxesPositions.emplace_back(inputFile);
	}
    
	// reading from file failed before EOF 
	// Abort because data seems to be invalid
	if (!inputFile.eof())
	{
		std::cout << "Error happened while reading boxes positions, Aborting!!\n";
		return;
	}
    
	// the final boxInfo read will always fail because of EOF 
	// so remove it 
	if (inputFile.fail())
		boxesPositions.pop_back();
    
	viewer->updateSolutionViewerWithGivenBoxes(boxesPositions);
}

//--------------------------------------------------------------------------------------------------
// Name:   on_quitButton_clicked
// Action: closes the window
//--------------------------------------------------------------------------------------------------
void ViewerOnlyWindow::on_quitButton_clicked()
{
	close();
}
