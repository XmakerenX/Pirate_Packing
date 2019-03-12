#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <sstream>
#include <Qt>
#include <QtGui>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLayout>
#include <QScrollArea>

//------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    colorDistributaion(0, 255)
{
    ui->setupUi(this);
	this->setWindowIcon(QIcon("favicon.ico"));

    std::chrono::high_resolution_clock::time_point beginning = std::chrono::high_resolution_clock::now();
    unsigned long int seed = beginning.time_since_epoch().count();
    generator.seed(seed);

	int containerWidth = 300;
	int containerHeight = 400;
	int items = 60;

    QWidget* centeralWidget =  this->centralWidget();
	// relocate the paper Container
	QWidget* paperContainer = centeralWidget->findChild<QWidget*>("paperContainer");
	QPoint paperPos = paperContainer->geometry().topLeft();
	paperContainer->setGeometry(paperPos.x(), paperPos.y(), containerWidth, containerHeight);
	QWidget* solutionViewer = centeralWidget->findChild<QWidget*>("label_2");
	//QPoint solSize = solutionViewer->geometry().bottomRight;
	solutionViewer->setGeometry(paperPos.x(), 210, containerWidth, 61);
	// relocate the paper
	QWidget* paper = paperContainer->findChild<QWidget*>("paper");
	paper->setGeometry(0, 0, containerWidth, containerHeight);
	// relocate the item container
	QWidget* itemContainer = centeralWidget->findChild<QWidget*>("ItemContainer");
	int itemContainerX = std::max(paperPos.x() + containerWidth + 30, 360);
	int itemContainerWidth = 1271 - itemContainerX;
	itemContainer->setGeometry(itemContainerX, 130, itemContainerWidth, 541);
	QWidget* partsTitle = centeralWidget->findChild<QWidget*>("PartsTitle");
	partsTitle->setGeometry(itemContainerX, 70, itemContainerWidth, 61);
	// relocate the scrollarea
	QWidget* scrollarea = itemContainer->findChild<QWidget*>("scrollAreaWidgetContents");
	scrollarea->setGeometry(0, 0, itemContainerWidth - 23, 539);
	// relocate the inner label
	QWidget* paper_2 = scrollarea->findChild<QWidget*>("paper_2");
	paper_2->setGeometry(0, 0, itemContainerWidth - 30, 531);

	legoPartsWidth = itemContainerWidth - 30;
	legoPartsHeight = 531;

    QPushButton* button = centeralWidget->findChild<QPushButton*>("pushButton");
    connect(button, SIGNAL(released()), this, SLOT(buttonClicked()));

    GA = new GAThread(containerWidth, containerHeight, items, legoPartsWidth);
	connect(GA, &GAThread::itemRectsUpdate, this, &MainWindow::updateItemViewer);
    connect(GA, &GAThread::rectsReady, this, &MainWindow::updateRectViewer);
	connect(GA, &GAThread::GAStarted, this, &MainWindow::updateGAStarted);
	connect(GA, &GAThread::GAFinished, this, &MainWindow::updateGAFinished);
}
//------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
//------------------------------------------------------------------------------------------
void MainWindow::buttonClicked()
{
    std::cout << "button clicked\n";
    GA->start();
}
//------------------------------------------------------------------------------------------
void MainWindow::updateItemViewer(int newHeight)
{
	const std::vector<Rect>& curRect = GA->getItemRects();

	QWidget* centeralWidget = this->centralWidget();
	QWidget* temp = centeralWidget->findChild<QWidget*>("ItemContainer");
	QWidget* itemDisplayArea = temp->findChild<QWidget*>("scrollAreaWidgetContents");
	QWidget* frame = itemDisplayArea->findChild<QWidget*>("paper_2");

	itemDisplayArea->setUpdatesEnabled(false);
	qDeleteAll(itemDisplayArea->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

	std::string styleSheet = "border: 1px solid rgb";
	for (int i = 0; i < curRect.size(); i++)
	{
		std::stringstream indexStream, colorStream;
		colorStream << "(" << (unsigned int)curRect[i].color.r << "," << (unsigned int)curRect[i].color.g << "," << (unsigned int)curRect[i].color.b << ")";
		indexStream << i;
		QLabel* rect = new QLabel(indexStream.str().c_str(), itemDisplayArea);
		rect->setGeometry(curRect[i].left, curRect[i].top, curRect[i].getWidth(), curRect[i].getHeight());
		rect->setFrameStyle(1);
		rect->setStyleSheet((styleSheet + colorStream.str() + " ;background-color: rgb" + colorStream.str()).c_str());
		rect->setAlignment(Qt::AlignCenter);
		rect->show();
	}

	// resize the display area if needed so it will become scrollable
	itemDisplayArea->setMinimumSize(QSize(legoPartsWidth, newHeight+2));
	if (newHeight > legoPartsHeight)
		frame->setGeometry(QRect(0, 0, legoPartsWidth, newHeight+2));
	else
		frame->setGeometry(QRect(0, 0, legoPartsWidth, legoPartsHeight));

	itemDisplayArea->setUpdatesEnabled(true);
}
//------------------------------------------------------------------------------------------
void MainWindow::updateRectViewer(int index, int overallMaximumFitness, double areaCovered, int currentGenPopulationFitness)
{
    const std::vector<std::vector<Rect>>& rectArray = GA->getRects();
    const std::vector<Rect>& curRect = rectArray[index];

    QWidget* centeralWidget =  this->centralWidget();
	QLabel* genOutput = centeralWidget->findChild<QLabel*>("GenOutput");
	genOutput->setText(QString::number(index + 1));
	QLabel* solutionFitnessOutput = centeralWidget->findChild<QLabel*>("solutionFitnessOutput");
	solutionFitnessOutput->setText(QString::number(overallMaximumFitness));
	QLabel* percOutput = centeralWidget->findChild<QLabel*>("percOutput");
	percOutput->setText(QString::number(areaCovered*100));
	QLabel* popFitnessOutput = centeralWidget->findChild<QLabel*>("popFitnessOutput");
	popFitnessOutput->setText(QString::number(currentGenPopulationFitness));

    QWidget* paper = centeralWidget->findChild<QWidget*>("paperContainer");

    paper->setUpdatesEnabled(false);
    qDeleteAll(paper->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

    std::string styleSheet = "border: 1px solid rgb";
    for (int i = 0; i < curRect.size(); i++)
    {
        std::stringstream indexStream, colorStream;
		colorStream << "(" << (unsigned int)curRect[i].color.r << "," << (unsigned int)curRect[i].color.g << "," << (unsigned int)curRect[i].color.b << ")";
        indexStream << i;
        QLabel* rect = new QLabel(indexStream.str().c_str(), paper);
        rect->setGeometry(curRect[i].left , curRect[i].top , curRect[i].getWidth(), curRect[i].getHeight());
        rect->setFrameStyle(1);
		rect->setStyleSheet((styleSheet + colorStream.str() + " ;background-color: rgb"+colorStream.str()).c_str());
        rect->setAlignment(Qt::AlignCenter);
        rect->show();
    }
    paper->setUpdatesEnabled(true);
}
//------------------------------------------------------------------------------------------
void MainWindow::updateGAStarted()
{
	QWidget* centeralWidget = this->centralWidget();
	QPushButton* button = centeralWidget->findChild<QPushButton*>("pushButton");
	button->setEnabled(false);
	QLabel* StatusOutput = centeralWidget->findChild<QLabel*>("StatusOutput");
	StatusOutput->setText("Running");
}
//------------------------------------------------------------------------------------------
void MainWindow::updateGAFinished()
{
	QWidget* centeralWidget = this->centralWidget();
	QPushButton* button = centeralWidget->findChild<QPushButton*>("pushButton");
	button->setEnabled(true);
	QLabel* StatusOutput = centeralWidget->findChild<QLabel*>("StatusOutput");
	StatusOutput->setText("Finished");
}