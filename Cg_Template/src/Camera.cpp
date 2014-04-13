#include <GL/glew.h>

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

void Camera::Update( float fDeltaTime )
{

}

void Camera::SetViewport( int x, int y, int width, int height )
{
    m_ViewportX = x;
    m_ViewportY = y;
    m_ViewportWidth = width;
    m_ViewportHeight = std::max( height, 1 );

    glViewport( m_ViewportX, m_ViewportY, m_ViewportWidth, m_ViewportHeight );

}

void Camera::SetProjection( float fovy, float aspect, float zNear, float zFar )
{
    m_fVFOV = fovy;
    m_fAspect = aspect;
    m_fNear = zNear;
    m_fFar = zFar;
    m_ProjectionMatrix = glm::perspective( fovy, aspect, zNear, zFar );
}

void Camera::AddPitch( float fPitch )
{
    // Add rotation about the X axis
    m_Rotate = m_Rotate * glm::angleAxis( glm::radians(fPitch), 1.0f, 0.0f, 0.0f );
    m_bViewDirty = true;
}

void Camera::AddYaw( float fYaw )
{
    // Add rotation about the Y axis
    m_Rotate = m_Rotate * glm::angleAxis( glm::radians(fYaw), 0.0f, 1.0f, 0.0f );
    m_bViewDirty = true;
}

void Camera::AddRoll( float fRoll )
{
    // Add rotation about the Z axis
    m_Rotate = m_Rotate * glm::angleAxis( glm::radians(fRoll), 0.0f, 0.0f, 1.0f );
    m_bViewDirty = true;
}

void Camera::AddRotation( const glm::quat& deltaRot )
{
    m_Rotate = m_Rotate * deltaRot;
    m_bViewDirty = true;
}

void Camera::TranslateX( float x )
{
    m_Translate.x += x;
    m_bViewDirty = true;
}

void Camera::TranslateY( float y )
{
    m_Translate.y += y;
    m_bViewDirty = true;
}

void Camera::TranslateZ( float z )
{
    m_Translate.z += z;
    m_bViewDirty = true;
}

void Camera::SetTranslate( const glm::vec3& translate )
{
    m_Translate = translate;
    m_bViewDirty = true;
}

void Camera::SetRotate( float pitch, float yaw, float roll )
{
    SetRotate( glm::vec3( pitch, yaw, roll ) );
}

void Camera::SetRotate( const glm::vec3& rotate )
{
    SetRotate( glm::quat( glm::radians( rotate ) ) );

}

void Camera::SetRotate( const glm::quat& rot )
{
    m_Rotate = rot;
    m_bViewDirty = true;
}

glm::vec3 Camera::GetTranslation() const
{
    return m_Translate;
}

glm::quat Camera::GetRotation() const
{
    return m_Rotate;
}

glm::vec3 Camera::GetEularAngles() const
{
    return glm::degrees( glm::eulerAngles( m_Rotate ) );
}

void Camera::ApplyViewport()
{
    glViewport( m_ViewportX, m_ViewportY, m_ViewportWidth, m_ViewportHeight );
}

void Camera::UpdateViewMatrix()
{
    if ( m_bViewDirty )
    {
        glm::mat4 translateMatrix = glm::translate( m_Translate );
        glm::mat4 rotationMatrix = glm::toMat4( m_Rotate );

        m_ViewMatrix = glm::inverse( rotationMatrix * translateMatrix );

        m_bViewDirty = false;
    }
}


void Camera::SetViewMatrix( const glm::mat4& viewMatrix )
{
    
    // Extract the translation
    m_Translate = glm::vec3( viewMatrix[3] );

    // Extract the top-left 3x3 matrix to decompose the scale and rotation
    glm::mat3 tmp = glm::mat3( viewMatrix );

    // TODO: I don't know if any of the scales are negative.
    // I have to figure out how can I reliably determine if any of the scales are negative?
    float sx = glm::length( tmp[0] );
    float sy = glm::length( tmp[1] );
    float sz = glm::length( tmp[2] );

    glm::mat3 invScale = glm::mat3( glm::scale( 1.0f/sx, 1.0f/sy, 1.0f/sz ) );
    // This will remove the scale factor (if there is one) so we can extract
    // the unit quaternion.
    tmp = tmp * invScale;

    m_Rotate  = glm::toQuat( tmp );

    // The view matrix needs to be rebuilt from the rotation and translation components.
    m_bViewDirty = true;
}

glm::mat4 Camera::GetViewMatrix()
{
    UpdateViewMatrix();
    return m_ViewMatrix;
}

void Camera::SetProjectionMatrix( const glm::mat4& projectionMatrix )
{
    // TODO: Decompose the projection matrix?
    m_ProjectionMatrix = projectionMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return m_ProjectionMatrix;
}
