#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "Cg/cgGL.h"
#include <string>
#include <vector>
#include <map>

typedef std::map<const char*, CGtechnique> TechniqueMap;

class Effect
{
public:

	virtual ~Effect();

	static Effect* Create(const std::string& a_Name, const std::string& a_File);

	const std::string& GetName() const {return m_Name;}

	bool Load(const std::string& a_File);
	bool Reload();

	CGpass				GetFirstPass();
	CGpass				GetNextPass();
	CGpass				GetCurrentPass() const { return m_CurrentPass; }
	CGtechnique			GetTechnique() const { return m_Technique; }
	CGtechnique			GetTechnique(const char* a_Name);
	CGeffect			GetCGEffect() const { return m_Effect; }
	void				UpdatePassParameters();
	void				SetCurrentTechnique(CGtechnique a_Technique) {m_Technique = a_Technique;}

	CGparameter			GetParameter(const char* param);
	void				SetParameter1f(const char* param, float val);
	void				SetParameter2f(const char* param, const float* val);
	void				SetParameter3f(const char* param, const float* val);
	void				SetParameter4f(const char* param, const float* val);
	void				SetParameter1i(const char* param, int val);
	void				SetParameter2i(const char* param, const int* val);
	void				SetParameter3i(const char* param, const int* val);
	void				SetParameter4i(const char* param, const int* val);
	void				SetParameter1d(const char* param, double val);
	void				SetParameter2d(const char* param, const double* val);
	void				SetParameter3d(const char* param, const double* val);
	void				SetParameter4d(const char* param, const double* val);
	void				SetParameterMatrix(const char* param, const float* val);
	void				SetParameterTexture(const char* param, unsigned int texture);

	void				Bind();
	void				Unbind();

protected:

	Effect(const std::string& a_Name, const std::string& a_File);

	std::string m_Name;
	std::string m_File;
	CGeffect m_Effect;
	CGtechnique m_Technique;
	CGpass m_CurrentPass;

	std::vector<CGtechnique> m_TechniqueList;

	static CGcontext m_Context;
};
#endif