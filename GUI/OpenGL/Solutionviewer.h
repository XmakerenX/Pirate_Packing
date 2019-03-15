#ifndef SOLUTIONVIEWER_H
#define SOLUTIONVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <vector>
#include "Mesh.h"
#include "Object.h"
#include "../../includes/structs.h"
#include "../GA/GAThread.h"

class SolutionViewer : public QOpenGLWidget
{
public:
    SolutionViewer(QWidget *parent);
    virtual ~SolutionViewer() override;
    //void updateSolutionViewer(const std::vector<BoxInfo>& boxesToShow);
    void updateSolutionViewer(GAThread* ga, int index);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLShaderProgram* m_boxesShader;

    QMatrix4x4 m_projection;

    Mesh* boxMesh;
    Mesh* containerMesh;
    QOpenGLTexture* containerTexture;
    QOpenGLTexture* boxTexture;

    std::vector<Object> m_boxes;

    Object* container;
};

#endif // SOLUTIONVIEWER_H
