#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // For OutputDebugString
#endif
#include <sstream>
#include "Effect.h"
#include <string>

CGcontext Effect::m_Context = NULL;

// Error handler for Cg
void CgErrorHandler( CGcontext context, CGerror error, void* appdata )
{
	if ( error != CG_NO_ERROR )
	{
		std::stringstream ss;
		const char* pStr = cgGetErrorString(error);
		std::string strError = ( pStr == NULL ) ? "" : pStr;
		ss << "Cg ERROR: " << strError << std::endl;

		std::string strListing;
		if ( error == CG_COMPILER_ERROR )
		{
			pStr = cgGetLastListing( context );
			strListing = ( pStr == NULL ) ? "" : pStr;

			ss << strListing << std::endl;
		}
#ifdef _WIN32
		OutputDebugStringA( ss.str().c_str() );
#else
		std::cerr << ss;
#endif
	}
}

Effect::Effect( const std::string& a_Name, const std::string& a_File )
{
	m_Name = a_Name;
	m_File = a_File;
	m_CurrentPass = NULL;

	if(!m_Context)
	{
		m_Context = cgCreateContext();
		cgSetErrorHandler(&CgErrorHandler, NULL);
		cgGLRegisterStates(m_Context);
		cgGLSetManageTextureParameters(m_Context, CG_TRUE);
	}

	Load(a_File);
}

Effect::~Effect()
{
	if(m_Effect != NULL)
	{
	 	cgDestroyEffect(m_Effect);
	 	m_Effect = NULL;
	}
	 
	if(m_Context != NULL)
	{
	 	cgDestroyContext(m_Context);
	 	m_Context = NULL;
	}
	 
	cgSetErrorHandler(NULL, NULL);
}

Effect* Effect::Create( const std::string& a_Name, const std::string& a_File )
{
	Effect* effect = new Effect(a_Name, a_File);

	return effect;
}

bool Effect::Load( const std::string& a_File )
{
	m_Effect = cgCreateEffectFromFile(m_Context, a_File.c_str(), NULL);
	
	if(m_Effect == NULL)
	{
		exit(-1);
	}

	CGtechnique technique = cgGetFirstTechnique(m_Effect);
	while(technique != NULL)
	{
		m_Technique = technique;
		m_TechniqueList.push_back(technique);
		technique = cgGetNextTechnique(technique);		
	}

	if(!m_Technique)
	{
		exit(-1);
	}

	return true;
}

bool Effect::Reload()
{
	cgDestroyEffect(m_Effect);
	m_TechniqueList.clear();
	Load(m_File);

	return true;
}

CGpass Effect::GetFirstPass()
{
	m_CurrentPass = cgGetFirstPass(m_Technique);
	return m_CurrentPass;
}

CGpass Effect::GetNextPass()
{
	m_CurrentPass = cgGetNextPass(m_CurrentPass);
	return m_CurrentPass;
}

void Effect::UpdatePassParameters()
{
	cgUpdatePassParameters(m_CurrentPass);
}

CGparameter Effect::GetParameter( const char* param )
{
	CGparameter p = cgGetEffectParameterBySemantic(m_Effect, param);
	if(p) return p;

	p = cgGetNamedEffectParameter(m_Effect, param);
	if(p) return p;

	return 0;
}

void Effect::SetParameter1f( const char* param, float val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter1f(parameter, val);
}

void Effect::SetParameter2f( const char* param, const float* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter2f(parameter, val[0], val[1]);
}

void Effect::SetParameter3f( const char* param, const float* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter3f(parameter, val[0], val[1], val[2]);
}

void Effect::SetParameter4f( const char* param, const float* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter4f(parameter, val[0], val[1], val[2], val[3]);
}

void Effect::SetParameter1i( const char* param, int val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter1i(parameter, val);
}

void Effect::SetParameter2i( const char* param, const int* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter2i(parameter, val[0], val[1]);
}

void Effect::SetParameter3i( const char* param, const int* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter3i(parameter, val[0], val[1], val[2]);
}

void Effect::SetParameter4i( const char* param, const int* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter4i(parameter, val[0], val[1], val[3], val[4]);
}

void Effect::SetParameter1d( const char* param, double val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter1d(parameter, val);
}

void Effect::SetParameter2d( const char* param, const double* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter2d(parameter, val[0], val[1]);
}

void Effect::SetParameter3d( const char* param, const double* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter3d(parameter, val[0], val[1], val[2]);
}

void Effect::SetParameter4d( const char* param, const double* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetParameter4d(parameter, val[0], val[1], val[3], val[4]);
}

void Effect::SetParameterMatrix( const char* param, const float* val )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgSetMatrixParameterfc(parameter, val);
}

void Effect::SetParameterTexture( const char* param, unsigned int texture )
{
	CGparameter parameter = GetParameter(param);
	if(parameter) cgGLSetTextureParameter(parameter, texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
}

void Effect::Bind()
{
	cgSetPassState(m_CurrentPass);
}

void Effect::Unbind()
{
	cgResetPassState(m_CurrentPass);
}

CGtechnique Effect::GetTechnique( const char* a_Name )
{
	for(int i = 0; i < m_TechniqueList.size(); ++i)
	{
		const char* name = cgGetTechniqueName(m_TechniqueList[i]);
		if(strcmp(name, a_Name) != 1)
			return m_TechniqueList[i];
	}
}
