#ifndef _Camera_h_
#define _Camera_h_

class Camera
{
public:
    Camera() : m_Translate(0), m_bViewDirty(true) {}
    virtual ~Camera() {};

    // Set viewport parameters
    void SetViewport( int x, int y, int width, int height );
    // Set projection prameters
    void SetProjection( float fovy, float aspect, float zNear, float zFar );

    // Add this pitch (rotation about the X-axis) in degrees
    // to the current camera's pitch 
    void AddPitch( float fPitch );
    // Add this yaw (rotation about the Y-axis) in degrees
    // to the current camera's yaw
    void AddYaw( float fYaw );
    // Add this roll (rotation about the Z-axis) in degrees 
    // to the current camera's roll
    void AddRoll( float fRoll );

    void AddRotation( const glm::quat& rot );

    // Translate the the camera's X-axis
    void TranslateX( float x );
    // Translate the camera's Y-axis
    void TranslateY( float y );
    // Translate the camera's Z-axis
    void TranslateZ( float z );

    // Set the current translation of the camera, replacing the current translation.
    void SetTranslate( const glm::vec3& translate );
    // Set the current rotation from eular angles in degrees, replacing the current rotation.
    void SetRotate( float pitch, float yaw, float roll );
    // Set the current rotation from eular angles in degrees, replacing the current rotation.
    void SetRotate( const glm::vec3& rotate );
    void SetRotate( const glm::quat& rot );

    glm::vec3 GetTranslation() const;
    glm::quat GetRotation() const;
    // Get the current rotation of the camera in eurlar angles (degrees)
    glm::vec3 GetEularAngles() const;

    virtual void Update( float fDeltaTime ) = 0;

    virtual void ApplyViewport();

    // Directly set the view matrix
    void        SetViewMatrix( const glm::mat4& viewMatrix );
    glm::mat4   GetViewMatrix();

    // Directly set the projection matrix
    void        SetProjectionMatrix( const glm::mat4& projectionMatrix );
    glm::mat4   GetProjectionMatrix();

protected:

    virtual void UpdateViewMatrix();

    // Viewport parameters
    int         m_ViewportX;
    int         m_ViewportY;
    int         m_ViewportWidth;
    int         m_ViewportHeight;

    // Projection parameters
    float       m_fVFOV;
    float       m_fAspect;
    float       m_fNear;
    float       m_fFar;

    // View space parameters
    glm::vec3   m_Translate;
    glm::quat   m_Rotate;
    glm::mat4   m_ViewMatrix;
    glm::mat4   m_ProjectionMatrix;

    // True if the view matrix needs to be updated.
    bool        m_bViewDirty;

private:

};

#endif // _Camera_h_