#include <string>
#include <glm/glm.hpp>
#include "Material.h"

Material::Material()
{
	m_ID = 0;

	m_DiffuseMap = 0;
	m_SpecularMap = 0;
	m_NormalMap = 0;
	m_EmissiveMap = 0;

	m_Name = "";

	m_Emissive = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_Diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Specular = glm::vec4(0.3f, 0.3f, 0.3f, 0.3f);
	m_Shininess = 64.0f;

	m_Reflection = 0.5f;
	m_Refraction = 0.66685f;
	m_Transmittance = 0.8f;

	m_FresnelPower = 4.0f;
	m_FresnelBias = 0.0f;
	m_FresnelScale = 1.0f;
	m_ETARatio = glm::vec3(1.1f, 1.2f, 1.3f);
}

Material::~Material()
{

}