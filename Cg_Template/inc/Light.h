#ifndef _LIGHT_H_
#define _LIGHT_H_

enum LightType
{
	PointLight,
	SpotLight,
	DirectionalLight
};

class Light
{
public:
	
	Light();
	~Light();

	LightType m_Type;
	glm::vec4 m_Color;
	glm::vec4 m_Position;
	glm::vec4 m_Direction;

	float m_ConstantAttenuation;
	float m_LinearAttenuation;
	float m_QuadraticAttenuation;

	float m_InnerCosAngle;
	float m_OuterCosAngle;

};

#endif