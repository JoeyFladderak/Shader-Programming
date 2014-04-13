#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include "PivotCamera.h"

PivotCamera::PivotCamera()
    : m_PivotPoint( 0 )
{

}

glm::vec3 PivotCamera::ProjectOntoUnitSphere( glm::ivec2 screenPos )
{
    float x, y, z;

    x = ( ( screenPos.x / (float)m_ViewportWidth ) * 2.0f ) - 1.0f;
    y = 1.0f - ( ( screenPos.y / (float)m_ViewportHeight ) * 2.0f );

    //Compute the square of the length of the vector to the point from the center
    float length2 = (x * x) + (y * y);

    //If the point is mapped outside of the sphere... (length > radius squared)
    if (length2 > 1.0f)
    {
        // Compute the inverse of the length to find the point on a unit sphere
        float invLength = 1.0f / sqrtf(length2);

        //Return the "normalized" vector, a point on the sphere
        x *= invLength;
        y *= invLength;
        z = 0.0f;
    }
    else    //Else it's on the inside
    {
        //Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
        z = sqrtf(1.0f - length2);
    }

    return glm::vec3( x, y, z );
}

// The point where the mouse was clicked in
// client screen space.
void PivotCamera::MouseClicked( const glm::ivec2& point2d )
{
    m_PreviousPoint = ProjectOntoUnitSphere( point2d );
}

void PivotCamera::MouseMoved( const glm::ivec2& point2d )
{
    glm::vec3 currentPoint = ProjectOntoUnitSphere(point2d);
    glm::vec3 axis = glm::cross( currentPoint, m_PreviousPoint );

    if ( glm::length2(axis) > 0.0f )
    {
        float fDot = glm::dot( m_PreviousPoint, currentPoint );
        glm::quat deltaRotate( fDot, axis );

        AddRotation( deltaRotate );
    }

    m_PreviousPoint = currentPoint;
}

void PivotCamera::Update( float fDeltaTime )
{
    base::Update(fDeltaTime);

}
void PivotCamera::TranslatePivotX( float x )
{
    m_PivotPoint += m_Rotate * glm::vec3( x * GetPivotScaleFactor(), 0, 0 );
    m_bViewDirty = true;
}

void PivotCamera::TranslatePivotY( float y )
{
    m_PivotPoint += m_Rotate * glm::vec3( 0, y * GetPivotScaleFactor(), 0 );
    m_bViewDirty = true;
}

void PivotCamera::TranslatePivotZ( float z )
{
    m_PivotPoint += m_Rotate * glm::vec3( 0, 0, z * GetPivotScaleFactor() );
    m_bViewDirty = true;
}

void PivotCamera::SetPivot(  const glm::vec3& pivot )
{
    m_PivotPoint = pivot;
    m_bViewDirty = true;
}

glm::vec3 PivotCamera::GetPivot() const
{
    return m_PivotPoint;
}

float PivotCamera::GetPivotScaleFactor() const
{
    return std::max( fabsf( m_Translate.z / ( m_ViewportWidth * 1.25f ) ), 0.0f );
}

void PivotCamera::UpdateViewMatrix()
{
    if ( m_bViewDirty )
    {
        glm::mat4 translateMatrix = glm::translate( glm::mat4(1.0f), m_Translate );
        glm::mat4 rotationMatrix = glm::toMat4( m_Rotate );
        glm::mat4 pivotMatrix = glm::translate( glm::mat4(1.0f), m_PivotPoint );

        m_ViewMatrix = glm::inverse( pivotMatrix * rotationMatrix * translateMatrix );

        m_bViewDirty = false;
    }
}

