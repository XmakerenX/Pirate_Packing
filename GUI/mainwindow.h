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

private slots:
	void on_radioButton_HybridGenetics_clicked();

	void on_radioButton_pureGenetics_clicked();

	void on_populationSizeTextBox_textChanged();

	void on_numberOfGenerationTextBox_textChanged(const QString &arg1);

	void on_elitisimSizeTextBox_textChanged(const QString &arg1);

	void on_mutationRateTextBox_textChanged(const QString &arg1);
	void on_confirmButton_clicked();
	void on_backButton_clicked();


private:
	Ui::MainWindow *ui;
    SolutionViewer* viewer;
    GAThread * GA;

	void setForms();
};

#endif // MAINWINDOW_H
