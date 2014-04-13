#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class Material
{
public:

	Material();
	~Material();

	int m_ID;

	unsigned int m_DiffuseMap;
	unsigned int m_SpecularMap;
	unsigned int m_NormalMap;
	unsigned int m_EmissiveMap;

	std::string m_Name;
	glm::vec4 m_Emissive;
	glm::vec4 m_Ambient;
	glm::vec4 m_Diffuse;
	glm::vec4 m_Specular;
	float m_Shininess;

	// reflection stuff
	float m_Reflection;
	float m_Refraction;
	float m_Transmittance;

	float m_FresnelPower;
	float m_FresnelScale;
	float m_FresnelBias;
	glm::vec3 m_ETARatio;
};
#endif