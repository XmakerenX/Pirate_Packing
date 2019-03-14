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
	delete ui->mainToolBar; // get rid of the useless tool bar


	//Set icon:
	std::string iconPath = GuiUtils::getFullPath("favicon.ico");
	this->setWindowIcon(QIcon(iconPath.c_str()));

	//Set image:
	std::string logoPath =  GuiUtils::getFullPath("PiratePackingLogo.png");
	ui->logo_image->setPixmap(QPixmap(logoPath.c_str()));
	ui->logo_image->setScaledContents(true);

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
	//sol = new solution_viewer_window(this);
	//sol->show();
	//this->hide();
}
