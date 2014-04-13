#include <glm/glm.hpp>
#include "FaceGroup.h"

FaceGroup::FaceGroup()
{
	m_Vertices.clear();

	m_VAO = 0;
	m_VertexBuffer = 0;
}

FaceGroup::~FaceGroup()
{
	
}