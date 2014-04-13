#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <string>
#include "FaceGroup.h"
#include "Material.h"
#include "importer/OpenMDL.h"
#include "common/OpenMDLTypes.h"
// Cg
#include <Cg/cgGL.h> // "Cg/cg.h" is implicitly included
#include "Effect.h"

class Material;
class FaceGroup;
class Mesh
{
public:

	Mesh(std::string a_File, std::string a_Name);
	~Mesh();

	void Load(std::string a_File);
	void LoadMaterials();
	void BindMaterial(std::string a_Name, unsigned int a_FaceGroup);

	void Render();
	void RenderFaceGroup(const FaceGroup& a_FaceGroup);
	void SetEffect(Effect* a_Effect);

	std::string m_Name;

	std::vector<FaceGroup> m_FaceGroupList;
	std::vector<Material> m_MaterialList;

	std::vector<std::pair<std::string, unsigned int>> m_TextureList;

	OpenMDL::Scene* m_Scene;

	Effect* m_Effect;
};
#endif