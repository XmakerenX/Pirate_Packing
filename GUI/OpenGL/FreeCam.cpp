#include "FreeCam.h"
#include <QtMath>
#include <utility>

//-----------------------------------------------------------------------------
// Name : FreeCam (constructor)
//-----------------------------------------------------------------------------
FreeCam::FreeCam()
{

}

//-----------------------------------------------------------------------------
// Name : FreeCam (copy constructor)
//-----------------------------------------------------------------------------
FreeCam::FreeCam(const Camera *pCamera)
{
    SetCameraDetails(pCamera);
}

//-----------------------------------------------------------------------------
// Name : SetCameraDetails()
//-----------------------------------------------------------------------------
void FreeCam::SetCameraDetails(const Camera *pCamera)
{
    if (!pCamera) return;

    m_vecPos = pCamera->GetPosition();
    m_vecRight = pCamera->GetRight();
    m_vecLook = pCamera->GetLook();
    m_vecUp = pCamera->GetUp();
    m_fFOV = pCamera->GetFOV();
    m_fNearClip = pCamera->GetNearClip();
    m_fFarClip = pCamera->GetFarClip();
    m_viewPort = pCamera->GetViewport();

    m_bViewDirty = true;
    m_bProjDirty = true;
    m_bFrustumDirty = true;

    m_rotateMatrix.setToIdentity();
}

//-----------------------------------------------------------------------------
// Name : Rotate ()
// Desc : Rotate the camera about the cameras local axis
//        perfroms a standard three vector rotation(avoids gimble-lock)
//-----------------------------------------------------------------------------
void FreeCam::Rotate(float x, float y, float z)
{
    QMatrix4x4 mtxRotate;
    // save the up vector before the x rotation in order for the y rotation to work correctly
    QVector3D upTemp;
    QVector3D lookTemp;

    upTemp = m_vecUp;
    lookTemp = m_vecLook;

    if (x != 0)
    {
        m_fPitch += x;

        if (m_fPitch > 89.0f)
        {
            x -= m_fPitch - 89.0f;
            m_fPitch = 89.0f;
        }

        if (m_fPitch < -89.0f)
        {
            x -= m_fPitch + 89.0f;
            m_fPitch = -89.0f;
        }

        // build rotation matrix
        mtxRotate.setToIdentity();
        mtxRotate.rotate(x, m_vecRight);

        m_rotateMatrix = std::move(mtxRotate);

        m_vecLook  = mtxRotate * m_vecLook;
        m_vecUp    = mtxRotate * m_vecUp;
        m_vecRight = mtxRotate * m_vecRight;
    } // end if pitch

    if (y != 0)
    {
        m_fYaw += y;

        if (m_fYaw > 360.0f)
        {
            y -= 360.0f;
            m_fYaw = 360.0f;
        }

        if (m_fYaw < 0.0f)
        {
            y += 360.0f;
            m_fYaw = 0.0f;
        }

        // build rotation matrix
        mtxRotate.setToIdentity();
        mtxRotate.rotate(y, upTemp);
        m_rotateMatrix *= mtxRotate;

        m_vecLook  = mtxRotate * m_vecLook;
        m_vecUp    = mtxRotate * m_vecUp;
        m_vecRight = mtxRotate * m_vecRight;

    } // end if yaw

    if (z != 0)
    {
        m_fRoll += z;
        if (m_fRoll > 20.0f)
        {
            z -= (m_fRoll -20.0f);
            m_fRoll = 20.0f;
        }

        if (m_fRoll < -20.0f)
        {
            z -= (m_fRoll + 20.0f);
            m_fRoll = -20.0f;
        }

        // build rotation matrix
        
        mtxRotate.setToIdentity();
        mtxRotate.rotate(z, m_vecLook);
        m_rotateMatrix *= mtxRotate;

        m_vecUp = mtxRotate * m_vecUp;
        m_vecRight = mtxRotate * m_vecRight;


    } // end if roll

    m_vecUp = QVector3D(0.0f, 1.0f, 0.0f);

    m_vecLook.normalize();
    m_vecRight = QVector3D::crossProduct(m_vecUp, m_vecLook);
    m_vecRight.normalize();
    m_vecUp = QVector3D::crossProduct(m_vecLook, m_vecRight);
    m_vecUp.normalize();

    m_bViewDirty = true;
    m_bFrustumDirty = true;
}

//-----------------------------------------------------------------------------
// Name : RotateAroundPoint ()
//-----------------------------------------------------------------------------
void FreeCam::RotateAroundPoint(const QVector3D &point, float xDistance, float yDistance)
{
    QVector3D vecUp = m_vecUp;

    float distanceFromOrigin = m_vecPos.length();
    m_vecPos += m_vecRight * xDistance;
    m_vecPos += m_vecUp * yDistance;
    
    // keep the camera at the same distanec form origin
    m_vecPos.normalize();
    m_vecPos = m_vecPos * distanceFromOrigin;

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.lookAt(m_vecPos, point, QVector3D(0.0f, 1.0f, 0.0f));

    m_vecRight = QVector3D(matrix(0,0) ,matrix(0,1), matrix(0,2));
    m_vecUp = QVector3D(0.0f, 1.0f, 0.0f);
    m_vecUp = QVector3D(matrix(1,0), matrix(1,1), matrix(1,2));
    m_vecLook = QVector3D(matrix(2,0), matrix(2,1), matrix(2,2));
    
    m_bViewDirty = true;
    m_bFrustumDirty = true;
}

//-----------------------------------------------------------------------------
// Name : Move ()
//-----------------------------------------------------------------------------
void FreeCam::Move(unsigned int direction, float distance)
{
    m_rightAxis = m_vecRight;
    m_rightAxis.setY(0);

    if (direction & Camera::DIR_FORWARD)
        m_vecPos += m_vecLook * (-distance);

    if (direction & Camera::DIR_BACKWARD)
        m_vecPos += m_vecLook * distance;

    if (direction & Camera::DIR_RIGHT)
        m_vecPos += m_rightAxis * distance;

    if (direction & Camera::DIR_LEFT)
        m_vecPos += m_rightAxis * (-distance);

    if (direction & Camera::DIR_UP)
        m_vecPos += m_vecUp * distance;

    if (direction & Camera::DIR_DOWN)
        m_vecPos += m_vecUp * (-distance);

    m_bViewDirty = true;
    m_bFrustumDirty = true;

}


//-----------------------------------------------------------------------------
// Name : getRotateMatrix ()
//-----------------------------------------------------------------------------
QMatrix4x4& FreeCam::getRotateMatrix()
{
    return m_rotateMatrix;
}
