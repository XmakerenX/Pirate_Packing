#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../GA/GAThread.h"
#include "OpenGL/Solutionviewer.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	private slots:
	void on_pushButton_2_clicked();

	void on_pushButton_3_clicked();
	void on_startButton_clicked();
    
	void updateGAStarted();
	void updateGAFinished();

public slots:
	void on_radioButton_HybridGenetics_clicked();
	void on_radioButton_pureGenetics_clicked();
	void on_confirmButton_clicked();
	void on_backButton_clicked();
    
private:
	Ui::MainWindow *ui;
    SolutionViewer* viewer;
    Dimensions containerDim;
    GAThread * GA;

	void setForms();
	void validateInput(std::string inputString);
	std::string readFileFromUser();
	void parseInput(std::string str);
};

#endif // MAINWINDOW_H
