#ifndef VIEWERONLYWINDOW_H
#define VIEWERONLYWINDOW_H

#include <QMainWindow>
#include "../GA/GAThread.h"
#include "OpenGL/Solutionviewer.h"

namespace Ui {
	class ViewerOnlyWindow;
}

class ViewerOnlyWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ViewerOnlyWindow(QWidget *parent = nullptr);
	~ViewerOnlyWindow();

public slots:        
	void on_loadFileButton_clicked();    
        void on_quitButton_clicked();
    
private:
	Ui::ViewerOnlyWindow *ui;
	SolutionViewer* viewer;
	Dimensions containerDim;
        GAThread * GA;

	void setForms();
};

#endif // VIEWERONLYWINDOW_H

