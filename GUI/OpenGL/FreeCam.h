#ifndef  _FREECAM_H
#define  _FREECAM_H

#include "Camera.h"

class FreeCam : public Camera
{
public:
    //-----------------------------------------------------------------------------
    // Constructors & Destructors
    //-----------------------------------------------------------------------------
    FreeCam();
    FreeCam(const Camera* pCamera);

    //-----------------------------------------------------------------------------
    // Public Functions for This Class.
    //-----------------------------------------------------------------------------
    void        Rotate            (float x, float y, float z);
    void        RotateAroundPoint (const QVector3D& point, float xDistance, float yDistance);
    void        SetCameraDetails  (const Camera* pCamera);
    void        Move              (unsigned int direction, float distance);
    QMatrix4x4& getRotateMatrix  ();

private:
    QVector3D m_rightAxis;
    QMatrix4x4 m_rotateMatrix;
};

#endif  //_FREECAM_H

