#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GUI_utils.h"
#include <QFileDialog>
#include <QPixmap>
#include <QString>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//Init UI:
	ui->setupUi(this);

	//Set icon:
	std::string iconPath = GuiUtils::getFullPath("favicon.ico");
	this->setWindowIcon(QIcon(iconPath.c_str()));

	//Set image:
	std::string logoPath =  GuiUtils::getFullPath("PiratePackingLogo.png");
	ui->logo_image->setPixmap(QPixmap(logoPath.c_str()));
	ui->logo_image->setScaledContents(true);
    
    viewer = new SolutionViewer(ui->page_2);
    viewer->setGeometry(QRect(210,130, 320, 240));
    
    // enable anit-aliasing so boxes will look less shit
    QSurfaceFormat format;
    format.setSamples(4);
    viewer->setFormat(format);
    
    viewer->show();
    
    GA = new GAThread(Dimensions(10,10,10), 10);
    connect(GA, &GAThread::boxesReady, viewer, &SolutionViewer::updateSolutionViewer);
    connect(GA, &GAThread::GAStarted, this, &MainWindow::updateGAStarted);
    connect(GA, &GAThread::GAFinished, this, &MainWindow::updateGAFinished);

}
//------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete ui;
}
//------------------------------------------------------------------------------------
void MainWindow::on_pushButton_2_clicked()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"), "/path/to/file/", tr("data Files (*.txt)"));
	//todo: make sure the user can only choose one file
	//todo: validate file

}
//------------------------------------------------------------------------------------
void MainWindow::on_pushButton_3_clicked()
{
    std::cout << "on_pushButton_3_clicked\n";
    QStackedWidget* stackedWidget = this->centralWidget()->findChild<QStackedWidget*>("stackedWidget");
    stackedWidget->setCurrentIndex(1);
}
//------------------------------------------------------------------------------------
void MainWindow::on_startButton_clicked()
{
    std::cout << "start button clicked\n";
    GA->start();
}
//------------------------------------------------------------------------------------
void MainWindow::updateGAStarted()
{
    ui->startButton->setEnabled(false);
}
//------------------------------------------------------------------------------------
void MainWindow::updateGAFinished()
{
    ui->startButton->setEnabled(true);
}
