#include "Solutionviewer.h"

//-----------------------------------------------------------------------------
// Name : SolutionViewer (constructor)
//-----------------------------------------------------------------------------
SolutionViewer::SolutionViewer(QWidget *parent)
{
    this->setParent(parent);

    m_boxesShader = nullptr;
    boxMesh = nullptr;
    containerMesh = nullptr;
    containerTexture = nullptr;
    boxTexture = nullptr;
    container = nullptr;
}

SolutionViewer::~SolutionViewer()
{
    // make sure that our opengl context is set
    makeCurrent();

    if (m_boxesShader)
        delete m_boxesShader;
    if (boxMesh)
        delete boxMesh;
    if (containerMesh)
        delete containerMesh;
    if (containerTexture)
        delete containerTexture;
    if (boxTexture)
        delete boxTexture;
    if (container)
        delete container;
}

//-----------------------------------------------------------------------------
// Name : mousePressEvent
//-----------------------------------------------------------------------------
void SolutionViewer::mousePressEvent(QMouseEvent* event) 
{
    oldMousePoint = event->pos();
    currentMousePoint = event->pos();
}

//-----------------------------------------------------------------------------
// Name : mouseMoveEvent
//-----------------------------------------------------------------------------
void SolutionViewer::mouseMoveEvent(QMouseEvent * event) 
{
    currentMousePoint = event->pos();
    
    float X = (currentMousePoint.x() - oldMousePoint.x()) / 3;
    float Y = (currentMousePoint.y() - oldMousePoint.y()) / 3;
    m_camera.RotateAroundPoint(QVector3D(0,0,0), X, Y);
    
    oldMousePoint = currentMousePoint;
    this->update();
}

//-----------------------------------------------------------------------------
// Name : mouseReleaseEvent
//-----------------------------------------------------------------------------
void SolutionViewer::mouseReleaseEvent(QMouseEvent * event)
{
    
}

//-----------------------------------------------------------------------------
// Name : updateSolutionViewer
//-----------------------------------------------------------------------------
void SolutionViewer::updateSolutionViewer(GAThread* ga, int index)
{
    m_boxes.clear();    
    std::vector<BoxInfo>& boxesToShow = (ga->getBoxesInfo())[index];
    
    for (const BoxInfo& curBoxInfo : boxesToShow)
    {
        QVector3D boxScale(curBoxInfo.boxWidth, curBoxInfo.boxHeight, curBoxInfo.boxLength);
        QVector3D pos = QVector3D(-10.0f, -10.0f, -10.0f) + curBoxInfo.startingPoint*2 + boxScale;
        m_boxes.emplace_back(pos,
                             QVector3D(0.0f, 0.0f, 0.0f),
                             boxScale,
                             curBoxInfo.color,
                             boxMesh);
    }
    
    this->update();
}

//-----------------------------------------------------------------------------
// Name : initializeGL
//-----------------------------------------------------------------------------
void SolutionViewer::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    // Set Render states
    f->glEnable(GL_CULL_FACE);
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load the cubes shader
    m_boxesShader = new QOpenGLShaderProgram();
    m_boxesShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shaders/box.vs");
    m_boxesShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shaders/box.frag");
    m_boxesShader->link();
    m_boxesShader->bind();

    // setup the box mesh
    std::vector<Vertex> boxVertices = {Vertex(QVector3D( 1.0f,-1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 0 0
                                        Vertex(QVector3D( 1.0f,-1.0f, 1.0f), QVector2D(0.0f, 1.0f)),  // 1 1
                                        Vertex(QVector3D(-1.0f,-1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 2 2
                                        Vertex(QVector3D(-1.0f,-1.0f,-1.0f), QVector2D(1.0f, 0.0f)),  // 3 3

                                        Vertex(QVector3D( 1.0f, 1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 4 4
                                        Vertex(QVector3D(-1.0f, 1.0f,-1.0f), QVector2D(0.0f, 1.0f)),  // 7 5
                                        Vertex(QVector3D(-1.0f, 1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 6 6
                                        Vertex(QVector3D( 1.0f, 1.0f, 1.0f), QVector2D(1.0f, 0.0f)),  // 5 7

                                        Vertex(QVector3D( 1.0f,-1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 0 8
                                        Vertex(QVector3D( 1.0f, 1.0f,-1.0f), QVector2D(0.0f, 1.0f)),  // 4 9
                                        Vertex(QVector3D( 1.0f, 1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 5 10
                                        Vertex(QVector3D( 1.0f,-1.0f, 1.0f), QVector2D(1.0f, 0.0f)),  // 1 11

                                        Vertex(QVector3D( 1.0f,-1.0f, 1.0f), QVector2D(0.0f, 0.0f)),  // 1 12
                                        Vertex(QVector3D( 1.0f, 1.0f, 1.0f), QVector2D(0.0f, 1.0f)),  // 5 13
                                        Vertex(QVector3D(-1.0f, 1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 6 14
                                        Vertex(QVector3D(-1.0f,-1.0f, 1.0f), QVector2D(1.0f, 0.0f)),  // 2 15

                                        Vertex(QVector3D(-1.0f,-1.0f, 1.0f), QVector2D(0.0f, 0.0f)),  // 2 16
                                        Vertex(QVector3D(-1.0f, 1.0f, 1.0f), QVector2D(0.0f, 1.0f)),  // 6 17
                                        Vertex(QVector3D(-1.0f, 1.0f,-1.0f), QVector2D(1.0f, 1.0f)),  // 7 18
                                        Vertex(QVector3D(-1.0f,-1.0f,-1.0f), QVector2D(1.0f, 0.0f)),  // 3 19

                                        Vertex(QVector3D( 1.0f, 1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 4 20
                                        Vertex(QVector3D( 1.0f,-1.0f,-1.0f), QVector2D(0.0f, 1.0f)),  // 0 21
                                        Vertex(QVector3D(-1.0f,-1.0f,-1.0f), QVector2D(1.0f, 1.0f)),  // 3 22
                                        Vertex(QVector3D(-1.0f, 1.0f,-1.0f), QVector2D(1.0f, 0.0f))}; // 7 23

    std::vector<unsigned int> boxIndices  = { 0,1,2, 0,2,3,
                                              4,5,6, 4,6,7,
                                              8,9,10, 8,10,11,
                                              12,13,14, 12,14,15,
                                              16,17,18, 16,18,19,
                                              20,21,22, 20,22,23};

    boxMesh = new Mesh(boxVertices, boxIndices, m_boxesShader);
    // setup the container mesh
    std::vector<Vertex> containerVertices = {Vertex(QVector3D( 1.0f, 1.0f,-1.0f), QVector2D(1.0f, 1.0f)),  // 0 0
                                             Vertex(QVector3D( 1.0f,-1.0f,-1.0f), QVector2D(1.0f, 0.0f)),  // 1 1
                                             Vertex(QVector3D(-1.0f, 1.0f,-1.0f), QVector2D(0.0f, 1.0f)),  // 2 2
                                             Vertex(QVector3D(-1.0f,-1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 3 3

                                             Vertex(QVector3D( 1.0f, 1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 4 4
                                             Vertex(QVector3D( 1.0f, 1.0f,-1.0f), QVector2D(1.0f, 0.0f)),  // 7 5
                                             Vertex(QVector3D(-1.0f, 1.0f, 1.0f), QVector2D(0.0f, 1.0f)),  // 6 6
                                             Vertex(QVector3D(-1.0f, 1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 5 7

                                             Vertex(QVector3D( 1.0f,-1.0f, 1.0f), QVector2D(1.0f, 0.0f)),  // 0 8
                                             Vertex(QVector3D( 1.0f,-1.0f,-1.0f), QVector2D(0.0f, 0.0f)),  // 4 9
                                             Vertex(QVector3D( 1.0f, 1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 5 10
                                             Vertex(QVector3D( 1.0f, 1.0f,-1.0f), QVector2D(0.0f, 1.0f)),  // 1 11

                                             Vertex(QVector3D(-1.0f, 1.0f, 1.0f), QVector2D(0.0f, 1.0f)),  // 1 12
                                             Vertex(QVector3D(-1.0f, 1.0f,-1.0f), QVector2D(1.0f, 1.0f)),  // 5 13
                                             Vertex(QVector3D(-1.0f,-1.0f, 1.0f), QVector2D(0.0f, 0.0f)),  // 6 14
                                             Vertex(QVector3D(-1.0f,-1.0f,-1.0f), QVector2D(1.0f, 0.0f)),  // 2 15

                                             Vertex(QVector3D( 1.0f,-1.0f, 1.0f), QVector2D(1.0f, 0.0f)),  // 2 16
                                             Vertex(QVector3D(-1.0f,-1.0f, 1.0f), QVector2D(0.0f, 0.0f)),  // 6 17
                                             Vertex(QVector3D( 1.0f,-1.0f,-1.0f), QVector2D(1.0f, 1.0f)),  // 7 18
                                             Vertex(QVector3D(-1.0f,-1.0f,-1.0f), QVector2D(0.0f, 1.0f)),  // 3 19

                                             Vertex(QVector3D(-1.0f,-1.0f, 1.0f), QVector2D(1.0f, 0.0f)),  // 4 20
                                             Vertex(QVector3D( 1.0f,-1.0f, 1.0f), QVector2D(0.0f, 0.0f)),  // 0 21
                                             Vertex(QVector3D(-1.0f, 1.0f, 1.0f), QVector2D(1.0f, 1.0f)),  // 3 22
                                             Vertex(QVector3D( 1.0f, 1.0f, 1.0f), QVector2D(0.0f, 1.0f))}; // 7 23

    std::vector<unsigned int> containerIndices  = { 0,2,3, 0,3,1,
                                                    4,6,7, 4,7,5,
                                                    8,10,11, 8,11,9,
                                                    12,14,15, 12,15,13,
                                                    16,18,19, 16,19,17,
                                                    20,22,23, 20,23,21};

    containerMesh = new Mesh(containerVertices, containerIndices, m_boxesShader);

    // load textures
    containerTexture = new QOpenGLTexture(QImage("resources/textures/container.png"));
    boxTexture = new QOpenGLTexture(QImage("resources/textures/box.png"));
    // init container object
    container = new Object(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(10.0f, 10.0f, 10.0f), QVector3D(0.0f, 0.0f, 0.0f), containerMesh);

    m_boxesShader->release();
    
    // init the camera
    m_camera.SetFOV(60.0f);
    m_camera.SetPostion(QVector3D( 20.0f, 20.0f, 20.0f));
    m_camera.SetLookAt(QVector3D( 0.0f, 0.0f, 0.0f));
}

//-----------------------------------------------------------------------------
// Name : resizeGL
//-----------------------------------------------------------------------------
void SolutionViewer::resizeGL(int w, int h)
{
    // set the camera projection matrix parameters
    m_camera.SetViewPort(0, 0, w, h, 0.01f, 100.0f); // doesn't really set viewPort
    //TODO: is there a need to set the viewport? or is it handled by Qt?
}

//-----------------------------------------------------------------------------
// Name : paintGL
//-----------------------------------------------------------------------------
void SolutionViewer::paintGL()
{
    // Draw the scene:
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_boxesShader->bind();
    QMatrix4x4 projectionViewMatrix = m_camera.GetProjMatrix() * m_camera.GetViewMatrix();
    
    // Render container
    containerTexture->bind();
    container->Draw(m_boxesShader, projectionViewMatrix);
    // Render boxes inside the container
    boxTexture->bind();

    for (Object& box : m_boxes)
    {
        box.Draw(m_boxesShader, projectionViewMatrix);
    }

    m_boxesShader->release();
}
