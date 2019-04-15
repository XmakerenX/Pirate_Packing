#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stack>
#include "../GA/GAThread.h"
#include "OpenGL/Solutionviewer.h"
#include "ItemTableModel.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void keyReleaseEvent(QKeyEvent *event);        
	void updateGAStarted();
	void updateGAFinished();
	void updateGuiDataCorrespondsToNewGeneration(int currentGen);
        
	void moveToMainMenu();
	void moveToSettings();
	void moveToEnterData(bool reset = true);
	void moveToViewer();
	void moveToPreviousPage();

public slots:
	void on_loadDataButton_clicked();
	void on_enterDataButton_clicked();
	void on_wumpusButton_clicked();
	void on_startButton_clicked();
	void on_resultsResetButton_clicked();

	void on_enterDataConfirmButton_clicked();
	void on_enterDataBackButton_clicked();        
        
	void on_radioButton_HybridGenetics_clicked();
	void on_radioButton_pureGenetics_clicked();
	void on_confirmButton_clicked();
	void on_backButton_clicked();
	void on_resultsBackButton_clicked();
	void on_generationComboBox_currentIndexChanged(QString indexStr);
        
	void on_saveConfiguration_clicked();
	void on_saveResults_clicked();
    
private:
	Ui::MainWindow *ui;
	SolutionViewer* viewer;
	Dimensions containerDim;
	GAThread * GA;
	ItemTableModel itemTable;
	std::stack<int> pageStack;

	void setForms();
	void validateInput(std::string inputString);
	std::string readFileFromUser();
	void parseInput(std::string str);
	void setFixedSizeAndMoveToCenter(int windowWidth, int windowHeight);


};

#endif // MAINWINDOW_H
