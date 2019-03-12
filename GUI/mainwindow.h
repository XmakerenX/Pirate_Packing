#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "../GA/GAThread.h"

namespace Ui {
class MainWindow;
class GAThread;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void updateItemViewer(int newHeight);
	void updateRectViewer(int index, int overallMaximumFitness, double areaCovered, int currentGenPopulationFitness);
	void updateGAStarted();
	void updateGAFinished();

public slots:
	void buttonClicked();

private:
	Ui::MainWindow *ui;
	GAThread* GA;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> colorDistributaion;
	int legoPartsWidth;
	int legoPartsHeight;
};

#endif // MAINWINDOW_H

