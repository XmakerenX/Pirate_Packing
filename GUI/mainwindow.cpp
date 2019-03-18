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

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//Init UI:
	ui->setupUi(this);

	setForms();


    // enable anit-aliasing so boxes will look less shit
    QSurfaceFormat format;
    format.setSamples(4);
    viewer->setFormat(format);
    viewer->show();
    Dimensions containerDim(10, 10, 10);
    viewer->setContainerDimensions(containerDim);
    GA = new GAThread(containerDim, 100);
    connect(GA, &GAThread::boxesReady, viewer, &SolutionViewer::updateSolutionViewer);
    connect(GA, &GAThread::GAStarted, this,    &MainWindow::updateGAStarted);
    connect(GA, &GAThread::GAFinished, this,   &MainWindow::updateGAFinished);

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
		ui->numberOfGenerationTextBox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->elitisimSizeTextBox->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));
		ui->mutationRateTextBox->setValidator(new QRegExpValidator(QRegExp("[0-9]\.[0-9]*"), this));
	
	//########################################################
	
	//Form - Solution 
		viewer = new SolutionViewer(ui->page_2);
		viewer->setGeometry(QRect(210, 130, 320, 240));


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
    std::cout << "Enter data button clicked\n";
    QStackedWidget* stackedWidget = this->centralWidget()->findChild<QStackedWidget*>("stackedWidget");
    stackedWidget->setCurrentIndex(2);
	this->setFixedSize(813, 837);
}
//------------------------------------------------------------------------------------
void MainWindow::on_backButton_clicked()
{
	std::cout << "Settings back button clicked\n";
	QStackedWidget* stackedWidget = this->centralWidget()->findChild<QStackedWidget*>("stackedWidget");
	
	stackedWidget->setCurrentIndex(0);
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


	boolean validSettings = true;
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
		else { GA_Settings::method = GA_Method::PureGenetic; }

		QStackedWidget* stackedWidget = this->centralWidget()->findChild<QStackedWidget*>("stackedWidget");
		stackedWidget->setCurrentIndex(1);
		this->setFixedSize(738, 539);
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

void MainWindow::on_populationSizeTextBox_textChanged()
{
	QString text = ui->populationSizeTextBox->text();

	if (text.length() >= 6)
	{
		ui->populationSizeTextBox->setText(text.mid(0, text.length() - 1));
	}
	if (text.startsWith("0"))
	{
		ui->populationSizeTextBox->setText(text.mid(1, text.length()));
	}

}
//--------------------------------------------------------------------------

void MainWindow::on_numberOfGenerationTextBox_textChanged(const QString &arg1)
{
	QString text = ui->numberOfGenerationTextBox->text();

	if (text.length() >= 6)
	{
		ui->numberOfGenerationTextBox->setText(text.mid(0, text.length() - 1));
	}
	if (text.startsWith("0"))
	{
		ui->numberOfGenerationTextBox->setText(text.mid(1, text.length()));
	}
}

void MainWindow::on_elitisimSizeTextBox_textChanged(const QString &arg1)
{
	QString text = ui->elitisimSizeTextBox->text();
	try {
		int a = text.toInt();
		if (a>100)
		{
			ui->elitisimSizeTextBox->setText(text.mid(0, text.length() - 1));
		}
	}
	catch (...)
	{

	}
	if (text.startsWith("0"))
	{
		ui->elitisimSizeTextBox->setText(text.mid(1, text.length()));
	}
}

void MainWindow::on_mutationRateTextBox_textChanged(const QString &arg1)
{
	QString text = ui->mutationRateTextBox->text();
	if (text == "") return;
	if (text == "0") return;
	if (text == "1") return;

	if (!text.startsWith("0"))
	{
		if (text.startsWith("1"))
		{
			ui->mutationRateTextBox->setText("1");
			return;
		}
		else
		{
			ui->mutationRateTextBox->setText("");
		}
	}
	else
	{
		if (text.startsWith("0."))
		{
			return;
		}
		else
		{
			ui->mutationRateTextBox->setText("0.");
		}
	}
}