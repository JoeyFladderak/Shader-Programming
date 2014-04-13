#include <glm/glm.hpp>
#include "Light.h"

Light::Light()
{
	m_Type = PointLight;
	m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_Direction = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	m_ConstantAttenuation = 1.0f;
	m_LinearAttenuation = 0.0f;
	m_QuadraticAttenuation = 0.0f;
	m_InnerCosAngle = -1.0f;
	m_OuterCosAngle = -1.0f;
}

Light::~Light()
{

}
