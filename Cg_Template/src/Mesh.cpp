#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLEW_STATIC // Link with static libs
#include <GL/glew.h>
#include <GL/wglew.h> // for WGL_EXT_swap_control
#define FREEGLUT_STATIC // Link with static libs
#include <GL/freeglut.h>
#include "Mesh.h"
#include <sstream>
#include "SOIL.h"

#define POSITION_ATTRIBUTE 0
#define NORMAL_ATTRIBUTE 2
#define DIFFUSE_ATTRIBUTE 3
#define TANGENT_ATTRIBUTE 14
#define BINORMAL_ATTRIBUTE 15
#define TEXCOORD0_ATTRIBUTE 8

#define MEMBER_OFFSET(s,m) ((char *)NULL + (offsetof(s,m)))
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

inline void CheckOpenGLError( const char* msg, const char* file, int line )
{
	GLenum errorCode = GL_NO_ERROR;
	while ( ( errorCode = glGetError() ) != GL_NO_ERROR )
	{
		std::stringstream ss;
		const GLubyte* errorString = gluErrorString(errorCode);

		if ( errorString != NULL )
		{

			ss << file << "(" << line << "): OpenGL Error: " << errorString << ": " << msg << std::endl;
#ifdef _WIN32
			OutputDebugStringA( ss.str().c_str() );
#else
			std::cerr << ss;
#endif
		}
	}
}

#ifdef _DEBUG
#define CheckGL() CheckOpenGLError( __FUNCSIG__, __FILE__, __LINE__ )
#else
#define CheckGL() ((void*)0) // Do nothing in release builds.
#endif 

Mesh::Mesh(std::string a_File, std::string a_Name)
{
	m_Name = a_Name;

	Load(a_File);

	for(unsigned int i = 0; i < m_FaceGroupList.size(); ++i)
	{
		glGenVertexArrays(1, &m_FaceGroupList[i].m_VAO); CheckGL();
		glBindVertexArray(m_FaceGroupList[i].m_VAO); CheckGL();

		glGenBuffers(1, &m_FaceGroupList[i].m_VertexBuffer); CheckGL();	
		glBindBuffer(GL_ARRAY_BUFFER, m_FaceGroupList[i].m_VertexBuffer); CheckGL();
		
		glBufferData(GL_ARRAY_BUFFER, m_FaceGroupList[i].m_Vertices.size() * sizeof(Vertex), &(m_FaceGroupList[i].m_Vertices[0]), GL_STATIC_DRAW); CheckGL();

		glVertexAttribPointer(POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, m_Position)); CheckGL();
		glEnableVertexAttribArray(POSITION_ATTRIBUTE); CheckGL();

		glVertexAttribPointer(NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, m_Normal)); CheckGL();
		glEnableVertexAttribArray(NORMAL_ATTRIBUTE); CheckGL();

		glVertexAttribPointer(TANGENT_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, m_Tangent)); CheckGL();
		glEnableVertexAttribArray(TANGENT_ATTRIBUTE); CheckGL();

		glVertexAttribPointer(BINORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, m_BiNormal)); CheckGL();
		glEnableVertexAttribArray(BINORMAL_ATTRIBUTE); CheckGL();
		 
 		glVertexAttribPointer(TEXCOORD0_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), MEMBER_OFFSET(Vertex, m_UV)); CheckGL();
 		glEnableVertexAttribArray(TEXCOORD0_ATTRIBUTE); CheckGL();
		 
		glBindBuffer(GL_ARRAY_BUFFER, 0); CheckGL();
		glBindVertexArray(0); CheckGL();

		BindMaterial(m_FaceGroupList[i].m_MatName, i);
	}
}

Mesh::~Mesh()
{
	for(int i = 0; i < m_FaceGroupList.size(); i++)
	{
		if(m_FaceGroupList[i].m_VAO != 0)
		{
			glDeleteVertexArrays(1, &m_FaceGroupList[i].m_VAO);
		}

		if(m_FaceGroupList[i].m_VertexBuffer != 0)
		{
			glDeleteBuffers(1, &m_FaceGroupList[i].m_VertexBuffer);
		}
	}
}

void Mesh::Load( std::string a_File )
{
	std::string folder = "data/Models/";

	m_Scene = OpenMDL::Importer::GetSingleton()->LoadScene((folder + a_File).c_str());

	unsigned int meshcount = m_Scene->GetMeshCount();
	for(unsigned int i = 0; i < meshcount; ++i)
	{
		OpenMDL::Mesh* mesh = m_Scene->GetMesh(i);
		unsigned int FGcount = mesh->GetFaceGroupCount();
		for(unsigned int j = 0; j < FGcount; ++j)
		{
			OpenMDL::FaceGroup* facegroup = mesh->GetFaceGroup(j);
			FaceGroup tmpFG;
			unsigned int indexcount = facegroup->GetIndexCount();
			for(unsigned int k = 0; k < indexcount; ++k)
			{
				unsigned int vertexIndex = facegroup->GetVertexIndex(k);
				unsigned int normalIndex = facegroup->GetNormalIndex(k);
				unsigned int uvIndex = facegroup->GetUVIndex(k);
				unsigned int tangentindex = facegroup->GetTangentIndex(k);

				OpenMDL::float3* vert = mesh->GetPoint(vertexIndex);
				OpenMDL::float3* normal = mesh->GetNormal(normalIndex);
				OpenMDL::float2* uv = mesh->GetUV(uvIndex);
				OpenMDL::float3* binormal = mesh->GetBinormal(normalIndex);
				OpenMDL::float3* tangent = mesh	->GetTangent(tangentindex);

				Vertex myvert;

				myvert.m_Position.x = vert->x;
				myvert.m_Position.y = vert->y;
				myvert.m_Position.z = vert->z;

				if(normal)
				{
					myvert.m_Normal.x = normal->x;
					myvert.m_Normal.y = normal->y;
					myvert.m_Normal.z = normal->z;
				}

				if(uv)
				{
					myvert.m_UV.x = uv->u;
					myvert.m_UV.y = uv->v;
				}

				if(binormal)
				{
					myvert.m_BiNormal.x = binormal->x;
					myvert.m_BiNormal.y = binormal->y;
					myvert.m_BiNormal.z = binormal->z;
				}

				if(tangent)
				{
					myvert.m_Tangent.x = tangent->x;
					myvert.m_Tangent.y = tangent->y;
					myvert.m_Tangent.z = tangent->z;
				}

				tmpFG.m_Vertices.push_back(myvert);
			}

			OpenMDL::Material* mat = facegroup->GetMaterial();
			tmpFG.m_MatName = mat->GetName().GetBuffer();
			m_FaceGroupList.push_back(tmpFG);
		}
	}

	LoadMaterials();
}

void Mesh::RenderFaceGroup( const FaceGroup& a_FaceGroup )
{
	glBindVertexArray(a_FaceGroup.m_VAO); CheckGL();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	unsigned int texid = 0;

	if(m_MaterialList[a_FaceGroup.m_MatID].m_DiffuseMap != 0)
	{
		texid = m_MaterialList[a_FaceGroup.m_MatID].m_DiffuseMap;
		m_Effect->SetParameterTexture("TEXUNIT0", texid);
		m_Effect->SetParameter1i("Material.DiffuseMap", 1);
	}
	else
	{
		m_Effect->SetParameterTexture("TEXUNIT0", 0);
		m_Effect->SetParameter4f("Material.Diffuse", glm::value_ptr(m_MaterialList[a_FaceGroup.m_MatID].m_Diffuse));
	}

	if(m_MaterialList[a_FaceGroup.m_MatID].m_SpecularMap != 0)
	{
		texid = m_MaterialList[a_FaceGroup.m_MatID].m_SpecularMap;
		m_Effect->SetParameterTexture("TEXUNIT1", texid);
		m_Effect->SetParameter1f("Material.Shininess", m_MaterialList[a_FaceGroup.m_MatID].m_Shininess);
		m_Effect->SetParameter1i("Material.SpecularMap", 1);
	}
	else
	{
		m_Effect->SetParameterTexture("TEXUNIT1", 0);
		m_Effect->SetParameter4f("Material.Specular", glm::value_ptr(m_MaterialList[a_FaceGroup.m_MatID].m_Specular));
		m_Effect->SetParameter1f("Material.Shininess", m_MaterialList[a_FaceGroup.m_MatID].m_Shininess);
	}

	if(m_MaterialList[a_FaceGroup.m_MatID].m_NormalMap != 0)
	{
		texid = m_MaterialList[a_FaceGroup.m_MatID].m_NormalMap;
		m_Effect->SetParameterTexture("TEXUNIT2", texid);
		m_Effect->SetParameter1i("Material.NormalMap", 1);
	}
	else
	{
		m_Effect->SetParameterTexture("TEXUNIT2", 0);
	}

	if(m_MaterialList[a_FaceGroup.m_MatID].m_EmissiveMap != 0)
	{
		texid = m_MaterialList[a_FaceGroup.m_MatID].m_EmissiveMap;
		m_Effect->SetParameterTexture("TEXUNIT3", texid);
		m_Effect->SetParameter1i("Material.EmissiveMap", 1);
	}
	else
	{
		m_Effect->SetParameterTexture("TEXUNIT3", 0);
		m_Effect->SetParameter4f("Material.Emissive", glm::value_ptr(m_MaterialList[a_FaceGroup.m_MatID].m_Emissive));
	}

	CGpass pass = m_Effect->GetFirstPass();
	while(pass)
	{
		m_Effect->Bind();
		glDrawArrays(GL_TRIANGLES, 0, a_FaceGroup.m_Vertices.size()); CheckGL();
		m_Effect->Unbind();

		pass = m_Effect->GetNextPass();
	}

	glBindVertexArray(0); CheckGL();
}

void Mesh::Render()
{
	for(unsigned int i = 0; i < m_FaceGroupList.size(); i++)
	{
		RenderFaceGroup(m_FaceGroupList[i]);
	}
}

void Mesh::LoadMaterials()
{
	unsigned int matcount = m_Scene->GetMaterialCount();
	for(unsigned int i = 0; i < matcount; ++i)
	{
		const OpenMDL::Material* mat = m_Scene->GetMaterial(i);
		Material tmpmat;

		tmpmat.m_Name = mat->GetName().GetBuffer();
		tmpmat.m_ID = i;

		if(mat->HasDiffuseMap())
		{
			char tmp[256];
			sprintf_s(tmp, "data/Models/%s", mat->GetDiffuseMap().GetBuffer());
			unsigned int id = SOIL_load_OGL_texture(tmp, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
			tmpmat.m_DiffuseMap = id;
		}

		if(mat->HasSpecularMap())
		{
			char tmp[256];
			sprintf_s(tmp, "data/Models/%s", mat->GetSpecularMap().GetBuffer());
			unsigned int id = SOIL_load_OGL_texture(tmp, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
			tmpmat.m_SpecularMap = id;
		}

		if(mat->HasNormalMap())
		{
			char tmp[256];
			sprintf_s(tmp, "data/Models/%s", mat->GetNormalMap().GetBuffer());
			unsigned int id = SOIL_load_OGL_texture(tmp, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
			tmpmat.m_NormalMap = id;
		}

		if(mat->HasEmissiveMap())
		{
			char tmp[256];
			sprintf_s(tmp, "data/Models/%s", mat->GetEmissiveMap().GetBuffer());
			unsigned int id = SOIL_load_OGL_texture(tmp, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
			tmpmat.m_EmissiveMap = id;
		}

		if((mat->GetAmbientColor().r + mat->GetAmbientColor().g + mat->GetAmbientColor().b) > 0)
		{
			tmpmat.m_Ambient.r = mat->GetAmbientColor().r;
			tmpmat.m_Ambient.g = mat->GetAmbientColor().g;
			tmpmat.m_Ambient.b = mat->GetAmbientColor().b;
		}

		if((mat->GetDiffuseColor().r + mat->GetDiffuseColor().g + mat->GetDiffuseColor().b) > 0)
		{
			tmpmat.m_Diffuse.r = mat->GetDiffuseColor().r;
			tmpmat.m_Diffuse.g = mat->GetDiffuseColor().g;
			tmpmat.m_Diffuse.b = mat->GetDiffuseColor().b;
		}

		if((mat->GetSpecularColor().r + mat->GetSpecularColor().g + mat->GetSpecularColor().b) > 0)
		{
			tmpmat.m_Specular.r = mat->GetSpecularColor().r;
			tmpmat.m_Specular.g = mat->GetSpecularColor().g;
			tmpmat.m_Specular.b = mat->GetSpecularColor().b;
		}

		if((mat->GetEmissiveColor().r + mat->GetEmissiveColor().g + mat->GetEmissiveColor().b) > 0)
		{
			tmpmat.m_Emissive.r = mat->GetEmissiveColor().r;
			tmpmat.m_Emissive.g = mat->GetEmissiveColor().g;
			tmpmat.m_Emissive.b = mat->GetEmissiveColor().b;
		}

		if(mat->GetSpecularCoeff() > 0)
			tmpmat.m_Shininess = mat->GetSpecularCoeff();

		m_MaterialList.push_back(tmpmat);
	}
}

void Mesh::BindMaterial( std::string a_Name, unsigned int a_FaceGroup )
{
	for(unsigned int i = 0; i < m_MaterialList.size(); ++i)
	{
		if(m_MaterialList[i].m_Name.compare(a_Name) == 0)
		{
			m_FaceGroupList[a_FaceGroup].m_MatID = m_MaterialList[i].m_ID;
		}
	}
}

void Mesh::SetEffect( Effect* a_Effect )
{
	if(a_Effect == m_Effect)
		return;

	m_Effect = a_Effect;
}
