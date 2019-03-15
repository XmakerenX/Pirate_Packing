#ifndef OBJECT_H
#define OBJECT_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include "Mesh.h"

class Object
{
public:
    Object(const QVector3D& pos, const QVector3D& angle, const QVector3D& scale, const QVector3D& color, Mesh* pMesh);
    void InitObject(const QVector3D& pos, const QVector3D& angle, const QVector3D& scale, const QVector3D& color, Mesh* pMesh);
    ~Object();

    const QMatrix4x4& GetWorldMatrix();
    QVector3D GetPosition();
    Mesh* GetMesh();

    void SetPos(const QVector3D& newPos);
    void SetRotAngles(const QVector3D& newRotAngles);
    void SetScale(const QVector3D& newScale);

    void Rotate(float x, float y, float z);
    void TranslatePos(float x ,float y, float z);

    void AttachMesh              (Mesh* pMesh);
    void SetObjectAttributes     (std::vector<unsigned int> meshAttribute);

    void Draw(QOpenGLShaderProgram* shader, const QMatrix4x4 &matViewProj);

private:
    void CalculateWorldMatrix();
    QVector3D  m_pos;
    QVector3D  m_color;
    QMatrix4x4 m_mthxWorld;
    QMatrix4x4 m_mthxInverseWorld;
    QMatrix4x4 m_mtxRot;
    QMatrix4x4 m_mtxScale;
    QVector3D  m_rotAngles;

    Mesh*       m_pMesh;
    bool        m_worldDirty;
};

#endif // OBJECT_H
