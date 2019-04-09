#include "Object.h"

//-----------------------------------------------------------------------------
// Name : Object (constructor)
//-----------------------------------------------------------------------------
Object::Object(const QVector3D& pos, const QVector3D& angle, const QVector3D& scale, const QVector3D& color, Mesh* pMesh)
{
    assert(pMesh);

    InitObject(pos, angle, scale, color, pMesh);
}

//-----------------------------------------------------------------------------
// Name : InitObject ()
//-----------------------------------------------------------------------------
void Object::InitObject(const QVector3D& pos, const QVector3D& angle, const QVector3D& scale, const QVector3D& color, Mesh* pMesh)
{
    m_color = color;
    SetPos(pos);
    SetRotAngles(angle);
    SetScale(scale);
    // update world matrix
    m_worldDirty = true;
    GetWorldMatrix();

    AttachMesh(pMesh);
}

//-----------------------------------------------------------------------------
// Name : Object (destructor)
//-----------------------------------------------------------------------------
Object::~Object()
{}

//-----------------------------------------------------------------------------
// Name : GetWorldMatrix
//-----------------------------------------------------------------------------
const QMatrix4x4& Object::GetWorldMatrix()
{
    if (m_worldDirty)
    {
        CalculateWorldMatrix();
    }

    return m_mthxWorld;
}

//-----------------------------------------------------------------------------
// Name : CalculateWorldMatrix
//-----------------------------------------------------------------------------
void Object::CalculateWorldMatrix()
{
    m_mthxWorld.setToIdentity();
    m_mthxWorld.translate(m_pos);
    m_mthxWorld = m_mthxWorld * m_mtxScale * m_mtxRot;
    m_mthxInverseWorld = m_mthxWorld;
    bool isInverted;
    m_mthxInverseWorld.inverted(&isInverted);
    m_worldDirty = false;
}


//-----------------------------------------------------------------------------
// Name : GetPosition
//-----------------------------------------------------------------------------
QVector3D Object::GetPosition()
{
    return m_pos;
}

//-----------------------------------------------------------------------------
// Name : GetMesh
//-----------------------------------------------------------------------------
Mesh* Object::GetMesh()
{
    return m_pMesh;
}

//-----------------------------------------------------------------------------
// Name : SetPos
//-----------------------------------------------------------------------------
void Object::SetPos(const QVector3D& newPos)
{
    m_pos = newPos;
    m_worldDirty = true;
}

//-----------------------------------------------------------------------------
// Name : SetRotAngles
//-----------------------------------------------------------------------------
void Object::SetRotAngles(const QVector3D& newRotAngles)
{
    m_rotAngles = newRotAngles;
    m_mtxRot.setToIdentity();
    m_mtxRot.rotate(m_rotAngles.x(), QVector3D(1.0f, 0.0f, 0.0f));
    m_mtxRot.rotate(m_rotAngles.y(), QVector3D(0.0f, 1.0f, 0.0f));
    m_mtxRot.rotate(m_rotAngles.z(), QVector3D(0.0f ,0.0f ,1.0f));

    m_worldDirty = true;
}

//-----------------------------------------------------------------------------
// Name : SetObjectScale
//-----------------------------------------------------------------------------
void Object::SetScale(const QVector3D& newScale)
{
    m_mtxScale.setToIdentity();
    m_mtxScale.scale(newScale);
    
    m_worldDirty = true;
}


//-----------------------------------------------------------------------------
// Name : Rotate
//-----------------------------------------------------------------------------
void Object::Rotate(float x, float y, float z)
{
    m_rotAngles.setX(x + m_rotAngles.x());
    m_rotAngles.setY(y + m_rotAngles.y());
    m_rotAngles.setZ(z + m_rotAngles.z());

    m_mtxRot.setToIdentity();
    m_mtxRot.rotate(m_rotAngles.x(), QVector3D(1.0f, 0.0f, 0.0f));
    m_mtxRot.rotate(m_rotAngles.y(), QVector3D(0.0f, 1.0f, 0.0f));
    m_mtxRot.rotate(m_rotAngles.z(), QVector3D(0.0f ,0.0f ,1.0f));

    m_worldDirty = true;
}

//-----------------------------------------------------------------------------
// Name : TranslatePos
//-----------------------------------------------------------------------------
void Object::TranslatePos(float x ,float y, float z)
{
    if (x != 0 || y != 0 || z != 0)
    {
        m_pos.setX(m_pos.x() + x);
        m_pos.setY(m_pos.y() + y);
        m_pos.setZ(m_pos.z() + z);

        m_worldDirty = true;
    }
}

//-----------------------------------------------------------------------------
// Name : attachMesh
//-----------------------------------------------------------------------------
void Object::AttachMesh(Mesh* pMesh)
{
    m_pMesh = pMesh;
}

//-----------------------------------------------------------------------------
// Name : Draw
//-----------------------------------------------------------------------------
void Object::Draw(QOpenGLShaderProgram* shader, const QMatrix4x4 &matViewProj)
{
    QMatrix4x4 temp =  matViewProj * GetWorldMatrix();
    shader->setUniformValue("projection", temp);
    shader->setUniformValue("matWorld", GetWorldMatrix());
    shader->setUniformValue("cubeColor", m_color);
    m_pMesh->Draw();
}
