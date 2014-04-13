#ifndef _PivotCamera_h_
#define _PivotCamera_h_

#include "Camera.h"

class PivotCamera : public Camera
{
public:
    typedef Camera base;

    PivotCamera();

    virtual void Update( float fDeltaTime );

    // point2d: The absolute position where the mouse was clicked in
    // client screen space.
    void MouseClicked( const glm::ivec2& point2d );
    // point2d: The absolute position of the mouse cursor
    void MouseMoved( const glm::ivec2& point2d );

    // Project a position in screen coordinates onto a unit sphere 
    // centered at the origin and return the projected point .
    glm::vec3 ProjectOntoUnitSphere( glm::ivec2 screenPos );

    void TranslatePivotX( float x );
    void TranslatePivotY( float y );
    void TranslatePivotZ( float z );

    void SetPivot( const glm::vec3& pivot );
    glm::vec3 GetPivot() const;

    // Returns the scale factor that is applied to the pivot point
    // so the model will generally move as fast as the mouse.
    float GetPivotScaleFactor() const;

protected:
    virtual void UpdateViewMatrix();

    // The point around which the camera will pivot.
    glm::vec3   m_PivotPoint;
    glm::vec3   m_PreviousPoint;

private:

};

#endif // _PivotCamera_h_