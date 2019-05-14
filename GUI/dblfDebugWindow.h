#ifndef DBLFDEBUGWINDOW_H
#define DBLFDEBUGWINDOW_H

#include <QMainWindow>
#include "../GA/GAThread.h"
#include "OpenGL/Solutionviewer.h"
#include "../GA/Encoding/PermutationCreature.h"

namespace Ui {
	class DBLFDebugWindow;
}

class DBLFDebugWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit DBLFDebugWindow(QWidget *parent = nullptr);
	~DBLFDebugWindow();

public slots:        
	void on_dblfOriginalButton_clicked();    
        void on_genChromosomeButton_clicked();
        void on_quitButton_clicked();
        
        void on_generationComboBox_currentIndexChanged(QString indexStr);
    
private:
	Ui::DBLFDebugWindow *ui;
	SolutionViewer* dblfImprovedViewer;
	SolutionViewer* dblfOriginalViewer;
	Dimensions containerDim;
	Configuration configuration;
	PermutationCreature testCreature;
        std::vector<std::vector<BoxInfo>> originalBoxes;

	void setForms();
};

#endif // DBLFDEBUGWINDOW_H

