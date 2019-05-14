#include "dblfDebugWindow.h"
#include "ui_dblfDebugWindow.h"
#include "includes/structs.h"
#include <QFileDialog>
#include <QString>
#include <QWidget>
#include <QApplication>
#include <iostream>
#include <fstream>

DBLFDebugWindow::DBLFDebugWindow(QWidget *parent) 
:	QMainWindow(parent),
	ui(new Ui::DBLFDebugWindow),
	containerDim(10, 10, 10),
	configuration(containerDim, 100),
	testCreature(&configuration)
{
	//Init UI:
	ui->setupUi(this);
	setForms();
}
//------------------------------------------------------------------------------------
DBLFDebugWindow::~DBLFDebugWindow()
{
	delete ui;
}
void DBLFDebugWindow::setForms()
{
	//Form - Solution 
		  dblfImprovedViewer = new SolutionViewer(ui->page_2);
		  dblfImprovedViewer->setGeometry(QRect(580, 250, 360, 360));
		  dblfImprovedViewer->setContainerDimensions(containerDim);
                  
                  dblfOriginalViewer = new SolutionViewer(ui->page_2);
                  dblfOriginalViewer->setGeometry(QRect(50, 250, 360, 360));
		  dblfOriginalViewer->setContainerDimensions(containerDim);
		// enable anit-aliasing so boxes will look less shit
		QSurfaceFormat format;
		// Force Qt to use OpenGL core profile
		// Qt is silly and uses by defualt the OpenGL Compatibility profile
		// this can cuase issues with some open source drivers
		format.setVersion(3,3); 
		format.setSamples(4);
		  dblfImprovedViewer->setFormat(format);
		  dblfImprovedViewer->show();
                  dblfOriginalViewer->setFormat(format);
		  dblfOriginalViewer->show();
}
//------------------------------------------------------------------------------------
void DBLFDebugWindow::on_dblfOriginalButton_clicked()
{
    std::cout << "dblf original button clicked\n";

    if (testCreature.getFitness() == 0)
        testCreature.calculateFittness();
    
    std::vector<BoxInfo> boxesPositions = testCreature.getBoxPositions();
    for (int i = 0; i < boxesPositions.size(); i++)
    {
        std::vector<BoxInfo> subBoxes;
        for (int j = 0; j < i; j++)
        {
            subBoxes.push_back(boxesPositions[j]);
        }
        originalBoxes.push_back(subBoxes);
        ui->generationComboBox->addItem(QString::number(i+1));
    }
    
    dblfOriginalViewer->updateSolutionViewerWithGivenBoxes(boxesPositions);
}
//------------------------------------------------------------------------------------
void DBLFDebugWindow::on_genChromosomeButton_clicked()
{
    std::cout << "generate Chromosome button clicked\n";
    ui->generationComboBox->clear();
    originalBoxes.clear();
    
    testCreature = PermutationCreature(&configuration);
    testCreature.calculateFittness();
    
    std::vector<BoxInfo> boxesPositions = testCreature.getBoxPositions();
    for (int i = 0; i < boxesPositions.size(); i++)
    {
        std::vector<BoxInfo> subBoxes;
        for (int j = 0; j <= i; j++)
        {
            subBoxes.push_back(boxesPositions[j]);
        }
        originalBoxes.push_back(subBoxes);
        ui->generationComboBox->addItem(QString::number(i+1));
    }
    
    ui->generationComboBox->setEnabled(true);
    ui->generationComboBox->setCurrentIndex(boxesPositions.size() - 1);
}
//------------------------------------------------------------------------------------
void DBLFDebugWindow::on_quitButton_clicked()
{
    close();
}

void DBLFDebugWindow::on_generationComboBox_currentIndexChanged(QString indexStr)
{
    	if (ui->generationComboBox->isEnabled()) 
	{
		int number = indexStr.toInt() - 1;
                std::cout << "index selected is " << number << "\n";
                if (number >= 0)
                    dblfOriginalViewer->updateSolutionViewerWithGivenBoxes(originalBoxes[number]);
	}
}
