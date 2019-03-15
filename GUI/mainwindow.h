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

private:
	Ui::MainWindow *ui;
    SolutionViewer* viewer;
    GAThread * GA;
};

#endif // MAINWINDOW_H
