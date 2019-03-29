#ifndef SOLUTIONVIEWER_H
#define SOLUTIONVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <vector>
#include "../../includes/structs.h"
#include "../../GA/GAThread.h"
#include "Mesh.h"
#include "Object.h"
#include "FreeCam.h"

class SolutionViewer : public QOpenGLWidget
{
public:
    SolutionViewer(QWidget *parent);
    virtual ~SolutionViewer() override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    
    void setContainerDimensions(Dimensions dim);
    void updateSolutionViewer(GAThread* ga, int index);
	void updateSolutionViewerWithGivenBoxes(std::vector<BoxInfo>& boxesToShow);


protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLShaderProgram* m_boxesShader;

    FreeCam m_camera;
    QPoint oldMousePoint;
    QPoint currentMousePoint;

    Mesh* boxMesh;
    Mesh* containerMesh;
    QOpenGLTexture* containerTexture;
    QOpenGLTexture* boxTexture;

    std::vector<Object> m_boxes;
    Dimensions containerDim;
    Object* container;
};

#endif // SOLUTIONVIEWER_H
