#ifndef  _CAMERA_H
#define  _CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
    //-----------------------------------------------------------------------------
    // Enumerators and Structures
    //-----------------------------------------------------------------------------
    enum DIRECTION{
        DIR_FORWARD     = 1,
        DIR_BACKWARD    = 2,
        DIR_LEFT        = 4,
        DIR_RIGHT       = 8,
        DIR_UP          = 16,
        DIR_DOWN        = 32,

        DIR_FORCE_32BIT = 0x7FFFFFFF
    };

    struct VIEWPORT
    {
        VIEWPORT()
        {
            x       = 0;
            y       = 0;
            width   = 0;
            height  = 0;
            minZ    = 0;
            maxZ    = 1;
        }

        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        float minZ;
        float maxZ;
    };

    struct PLANE3D
    {
      float a;
      float b;
      float c;
      float d;
    };

    //-----------------------------------------------------------------------------
    // Constructors & Destructors
    //-----------------------------------------------------------------------------
    Camera();
    Camera(const Camera * pCamera);
    virtual ~Camera();

    //-----------------------------------------------------------------------------
    // Public Functions
    //-----------------------------------------------------------------------------
    void SetFOV     (float FOV) {m_fFOV = FOV; m_bProjDirty = true;}
    void SetViewPort(long left, long top, long width, long height,
                     float nearClip, float farclip);
    void SetLookAt  (const QVector3D& vecLookat);
    void SetLookAt  (const QVector3D& vecLookat, const QVector3D& vecUp);

    const QMatrix4x4& GetProjMatrix     ();

    float             GetFOV            () const {return m_fFOV;}
    float             GetNearClip       () const {return m_fNearClip;}
    float             GetFarClip        () const {return m_fFarClip;}
    const VIEWPORT&   GetViewport       () const { return m_viewPort; }

    const QVector3D   GetPosition       () const {return m_vecPos;}
    const QVector3D   GetLook           () const {return m_vecLook;}
    const QVector3D   GetUp             () const {return m_vecUp;}
    const QVector3D   GetRight          () const {return m_vecRight;}
    const QMatrix4x4& GetViewMatrix     ();

    //-----------------------------------------------------------------------------
    // Public Virtual Functions
    //-----------------------------------------------------------------------------
    virtual void SetPostion(const QVector3D& position);
    virtual void Move      (unsigned int direction, float distance) = 0;
    virtual void Rotate    (float x, float y, float z) = 0;
    virtual void SetCameraDetails (const Camera* pCamera) = 0;

    bool BoundsInFrustum( const QVector3D& min, const QVector3D& max);

protected:
    //-----------------------------------------------------------------------------
    // Protected Functions
    //-----------------------------------------------------------------------------
    void CalcFrustumPlanes();

    //-----------------------------------------------------------------------------
    // Protected Variables
    //-----------------------------------------------------------------------------
    QMatrix4x4 m_mtxView; // Cached view matrix
    QMatrix4x4 m_mtxProj; // Cached projection matrix
    PLANE3D m_Frustum[6];

    bool m_bViewDirty;
    bool m_bProjDirty;
    bool m_bFrustumDirty;

    // Perspective Projection parameters
    float m_fFOV;
    float m_fNearClip;
    float m_fFarClip;
    VIEWPORT m_viewPort;

    // Camera current position & orientation
    QVector3D m_vecPos;
    QVector3D m_vecUp;
    QVector3D m_vecLook;
    QVector3D m_vecRight;

    float m_fPitch;
    float m_fRoll;
    float m_fYaw;
};

#endif  //_CAMERA_H

