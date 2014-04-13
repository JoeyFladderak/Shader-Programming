#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // For OutputDebugString
#endif

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// NOTE: The GLEW header file must come before the GLUT header
#define GLEW_STATIC // Link with static libs
#include <GL/glew.h>
#include <GL/wglew.h> // for WGL_EXT_swap_control
#define FREEGLUT_STATIC // Link with static libs
#include <GL/freeglut.h>

// Cg
#include <Cg/cgGL.h> // "Cg/cg.h" is implicitly included

// Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>


#include "ElapsedTime.h"
#include "PivotCamera.h"
#include "SOIL.h"
#include "Material.h"
#include "Light.h"
#include "Mesh.h"
#include "Effect.h"

std::vector<Mesh*> g_MeshList;
int g_CurrentMesh = 0;

Light g_Light;
Material g_Material;

std::vector<Effect*> g_FXList;
int g_CurrentFX = 0;

const char* g_ApplicationName = "Joey Fladderak - 090004 - GP4";

const unsigned int g_uiWindowWidth = 1280;
const unsigned int g_uiWindowHeight = 720;

// As of OpenGL 3.1 the matrix stack has been removed
// from the specification.
// We need to keep track of our own matrices now.
// Global Variables Matrices
glm::mat4 g_ProjectionMatrix;
glm::mat4 g_ViewMatrix;
glm::mat4 g_ModelMatrix;

PivotCamera g_Camera;
bool g_bLeftMouseDown = false;
bool g_bMiddleMouseDown = false;
bool g_bRightMouseDown = false;

glm::ivec2 g_PreviousMousePosition;

glm::vec3 g_DefaultCameraPosition( 0, 10, 40 );

bool m_bEnableAnimation = false;
bool m_bEnableLightRotation = false;

GLuint  g_EnvCubeMap = 0;
GLuint  g_ProjTexture = 0;

float g_DT;

// projective textures and shadow mapping variables
float g_fLightNearPlane = 0.1f;
float g_fLightFarPlane = 500.0f;
float g_fLightFoV = 110.0f;

glm::mat4 g_LightViewMatrix;
glm::mat4 g_LightProjectionMatrix;

glm::mat4 g_TextureBiasMatrix;
glm::mat4 g_TextureMatrix;

GLuint g_ShadowFBO = 0;
GLuint g_ShadowBufferWidth = 2048;
GLuint g_ShadowBufferHeight = 2048;
GLuint g_DepthBufferID = 0;
GLuint g_ColorBufferID = 0;

void InitGLUT( int argc, char* argv[] );
void InitGLEW();
void InitGL();
void InitCG();

// GLUT callbacks
// Invoked when we need to redraw the screen.
void OnDisplay();
// Invoked when there are no inputs to process from the user.
void OnIdle();
// Invoked when the window changes size.
void OnReshape( int width, int height );
// Invoked when the user presses a key on the keyboard.
void OnKeyPressed( unsigned char key, int x, int y );
// Invoked when the user releases the key on the keyboard.
void OnKeyReleased( unsigned char key, int x, int y );
// Invoked when the user presses a special key (non-ascii) on the keyboard.
void OnSpecialKeyPressed( int key, int x, int y );
// Invoked when the user releases a special key (non-ascii) on the keyboard.
void OnSpecialKeyReleased( int key, int x, int y );
// Invoked when the user presses or releases the mouse button on the screen.
void OnMouseButton( int button, int state, int x, int y );
// Invoked when the user scrolls the mouse wheel.
void OnMouseWheel( int button, int state, int x, int y );
// Invoked when the user moves the mouse in the screen while any mouse button is pressed.
void OnMouseMotion( int x, int y );
// Invoked when the user moves the mouse in the screen while no mouse buttons are pressed.
void OnPassiveMouseMotion( int x, int y );

void DrawCubeMap( GLuint texID );

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

void OnClose();

int main( int argc, char* argv[] )
{
	InitGLUT( argc, argv );
	InitGLEW();
	InitGL();
	InitCG();

	g_ModelMatrix = glm::mat4(1.0f);
	g_Camera.SetTranslate( g_DefaultCameraPosition );

	// Start processing our event loop.
	glutMainLoop();

	return 0;
}

void InitGLUT( int argc, char* argv[] )
{
	glutInit( &argc, argv ); 
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	// Create an OpenGL 3.1 forward compatible context.
	// If you are targeting earlier hardware or you don't have an 
	// OpenGL 3.1 compatible video card, simply remove the next two lines.
	glutInitContextVersion( 3, 1 );
	// NOTE: Cg 3.1 does not support forward-compatible contexts!
	//    glutInitContextFlags( GLUT_FORWARD_COMPATIBLE );

	glutInitWindowSize( g_uiWindowWidth, g_uiWindowHeight );

	int screenWidth = glutGet( GLUT_SCREEN_WIDTH );
	int screenHeight = glutGet( GLUT_SCREEN_HEIGHT );

	// Center the new window relative to the screen.
	glutInitWindowPosition( (screenWidth - g_uiWindowWidth)/2, (screenHeight - g_uiWindowHeight)/2 );

	// Creating the window will also create the OpenGL context.
	glutCreateWindow( g_ApplicationName );

	// If the window is closed, return control to the main method.
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

	// Register GLUT callbacks
	glutDisplayFunc(&OnDisplay);
	glutIdleFunc(&OnIdle);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&OnKeyPressed);
	glutKeyboardUpFunc(&OnKeyReleased);
	glutSpecialFunc(&OnSpecialKeyPressed);
	glutSpecialUpFunc(&OnSpecialKeyReleased);
	glutMouseFunc(&OnMouseButton);
	glutMouseWheelFunc(&OnMouseWheel);
	glutMotionFunc(&OnMouseMotion);
	glutPassiveMotionFunc(&OnPassiveMouseMotion);
	glutCloseFunc(&OnClose);

	CheckGL();
}

void InitGLEW()
{
	glewExperimental = GL_TRUE;
	if ( glewInit() != GLEW_OK )
	{
		std::cerr << "There was a problem initializing GLEW. Exiting..." << std::endl;
		exit(-1);
	}

	// Check for 3.1 support.
	// I've specified that a 3.1 forward-compatible context should be created.
	// so this parameter check should always pass if our context creation passed.
	// If we need access to deprecated features of OpenGL, we should check
	// the state of the GL_ARB_compatibility extension.
	if ( !GLEW_VERSION_3_1 )
	{
		std::cerr << "OpenGL 3.1 required version support not present." << std::endl;
		exit(-1);
	}

	if ( WGLEW_EXT_swap_control )
	{
		wglSwapIntervalEXT(0); // Disable vertical sync
	}

	CheckGL();
}

void InitGL()
{
	// Clear any accumulated OpenGL errors.
	while ( glGetError() != GL_NO_ERROR );

	// Output interesting information about the platform we are using
	std::cout << "Vendor: " << glGetString (GL_VENDOR) << std::endl; CheckGL();
	std::cout << "Renderer: " << glGetString (GL_RENDERER) << std::endl; CheckGL();
	std::cout << "Version: " << glGetString (GL_VERSION) << std::endl; CheckGL();
	std::cout << "GLSL: " << glGetString (GL_SHADING_LANGUAGE_VERSION) << std::endl; CheckGL();

	// Initialize some default OpenGL states
	glClearColor( 0.0f, 0.0f, 0.0f, 1 ); CheckGL();
	glClearDepth( 1.0 ); CheckGL();

	glEnable( GL_DEPTH_TEST ); CheckGL();
	glEnable( GL_CULL_FACE ); CheckGL();

 	g_EnvCubeMap = SOIL_load_OGL_cubemap( "data/Textures/mars/mars_west.jpg",
 		"data/Textures/mars/mars_east.jpg", 
 		"data/Textures/mars/mars_up.jpg",
 		"data/Textures/mars/mars_down.jpg",
 		"data/Textures/mars/mars_south.jpg",
 		"data/Textures/mars/mars_north.jpg",
 		SOIL_LOAD_AUTO,
 		SOIL_CREATE_NEW_ID,
 		SOIL_FLAG_MIPMAPS );
 
 	// To prevent artifacts at the edges, use clamping at texture bounds.
 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
 	// Make sure we unbind and disable the cube map texture.
 	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
 	glDisable( GL_TEXTURE_CUBE_MAP );
 
 	// Load a brushed metal texture for the reflection material
 	g_ProjTexture = SOIL_load_OGL_texture( "data/Textures/texture.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
 
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
 
 	glBindTexture( GL_TEXTURE_2D, 0 );
 	glDisable( GL_TEXTURE_2D );
 
 	g_MeshList.push_back(new Mesh("imrod2.omd", "modellol"));

 	g_ViewMatrix = g_Camera.GetViewMatrix(); CheckGL();
 
 	g_Light.m_Position.x = 10.0f;
 	g_Light.m_Position.y = 15.0f;
 	g_Light.m_Position.z = 40.0f;
   	g_Light.m_InnerCosAngle = 0.75f;
   	g_Light.m_OuterCosAngle = 0.65f;
 
 	static const glm::vec3 white(1,1,1);
 
 	g_LightProjectionMatrix = glm::perspective(g_fLightFoV, float(g_ShadowBufferWidth) / (float)g_ShadowBufferHeight, g_fLightNearPlane, g_fLightFarPlane);
 	g_TextureBiasMatrix = glm::scaleBias(0.5f, 0.5f);
 
 	glGenFramebuffers(1, &g_ShadowFBO);
 	glBindFramebuffer(GL_FRAMEBUFFER, g_ShadowFBO);
 
 	glGenTextures(1, &g_DepthBufferID);
 	glBindTexture(GL_TEXTURE_2D, g_DepthBufferID);
 	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_ShadowBufferWidth, g_ShadowBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
 
 	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(white) );
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
 
 	// Attach the depth texture to the depth target for the frame buffer.
 	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_DepthBufferID, 0 );
 
 	glGenTextures( 1, &g_ColorBufferID );
 	glBindTexture( GL_TEXTURE_2D, g_ColorBufferID );
 	glTexImage2D( GL_TEXTURE_2D, 0, 4, g_ShadowBufferWidth, g_ShadowBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL );
 
 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
 	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_ColorBufferID, 0 );
 
 	GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
 	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
 	{
 		std::cout << "Incomplete Framebuffer status." << std::endl;
 	}
 
 	// Unbind the buffers so we can render normally
 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
 	glBindTexture( GL_TEXTURE_2D, 0 );
}

void InitCG()
{
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/Lighting.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/Directional.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/SpotLight.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/Reflection.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/Refraction.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/ReflectRefract.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/Fresnel.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/NormalMapping.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/ProjectiveTexture.cgfx"));
 	g_FXList.push_back(Effect::Create("test", "data/Shaders/ShadowMapping.cgfx"));
}

void OnIdle()
{
 	static ElapsedTime elapsedTime;
 	const float fRotationRate = 0.5f; // Rotations per second.
 
 	g_DT = elapsedTime.GetElapsedTime();
 	static float circler = 0.0f;
 
 	if ( m_bEnableAnimation ) // Rotate the object.
 	{
 		g_ModelMatrix = glm::rotate( g_ModelMatrix, ( fRotationRate * 360.0f ) * g_DT, glm::vec3(0,1,0) );
 	}
 
 	if(m_bEnableLightRotation)
 	{
 		circler += g_DT;
   		g_Light.m_Direction.x = (1.0f * sinf( circler )); 
   		g_Light.m_Direction.z = (1.0f * cosf( circler ) );
 	}

	glutPostRedisplay();
}


void OnKeyPressed( unsigned char key, int x, int y )
{
	switch (key)
	{
	case 27:    // ESC
		{
			glutLeaveMainLoop();
		}
		break;
 	case ' ':
 		{
 			m_bEnableAnimation = !m_bEnableAnimation;
 		}
 		break;
 	case 'w':
 	case 'W':
 		{
			g_ModelMatrix = glm::translate(g_ModelMatrix, 0.0f, 0.0f, 1.0f);
 			//g_Light.m_Position.z -= 250.0f * g_DT;
 		}
 		break;
 	case 's':
 	case 'S':
 		{
			g_ModelMatrix = glm::translate(g_ModelMatrix, 1.0f, 0.0f, -1.0f);
 			//g_Light.m_Position.z += 250.0f * g_DT;
 		}
 		break;
 	case 'a':
 	case 'A':
 		{
			g_ModelMatrix = glm::translate(g_ModelMatrix, 1.0f, 0.0f, 0.0f);
 			//g_Light.m_Position.x -= 250.0f * g_DT;
 		}
 		break;
 	case 'd':
 	case 'D':
 		{
			g_ModelMatrix = glm::translate(g_ModelMatrix, -1.0f, 0.0f, 0.0f);
 			//g_Light.m_Position.x += 250.0f * g_DT;
 		}
 		break;
 	case 'q':
 	case 'Q':
 		{
 			g_Light.m_Position.y += 250.0f * g_DT;
 		}
 		break;
 	case 'e':
 	case 'E':
 		{
 			g_Light.m_Position.y -= 250.0f * g_DT;
 		}
 		break;
 	case 'f':
 	case 'F':
 		{
 			m_bEnableLightRotation = !m_bEnableLightRotation;
 		}
 		break;
 	case 'r':
 	case 'R':
 		{
 			// Reset model matrix
 			g_ModelMatrix = glm::mat4(1);
 			// Reset the view matrix.
 			g_Camera.SetTranslate( g_DefaultCameraPosition );
 			g_Camera.SetRotate( 0, 0, 0 );
 			g_Camera.SetPivot( glm::vec3(0) );
 		}
 		break;
 	case '0':
 		{
 			g_CurrentFX = 0;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '1':
 		{
 			g_CurrentFX = 1;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '2':
 		{
 			g_CurrentFX = 2;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '3':
 		{
 			g_CurrentFX = 3;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '4':
 		{
 			g_CurrentFX = 4;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '5':
 		{
 			g_CurrentFX = 5;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '6':
 		{
 			g_CurrentFX = 6;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '7':
 		{
 			g_CurrentFX = 7;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '8':
 		{
 			g_CurrentFX = 8;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '9':
 		{
 			g_CurrentFX = 9;
 			g_FXList[g_CurrentFX]->Reload();
 		}
 		break;
 	case '[':
 		{
 			if((g_CurrentMesh > 0))
 				g_CurrentMesh--;
 		}
 		break;
 	case ']':
 		{
 			if(g_CurrentMesh < g_MeshList.size() - 1)
 				g_CurrentMesh += 1;
 		}
 		break;
 	}
}

void OnReshape( int width, int height )
{
	if ( height < 1 ) height = 1;

	// Full-screen view port
	g_Camera.SetViewport( 0, 0, width, height );
	g_Camera.SetProjection( 45.0f, width/(float)height, 0.1f, 1000000.0f );
	g_ProjectionMatrix = g_Camera.GetProjectionMatrix();

	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( glm::value_ptr(g_ProjectionMatrix ) );
}

void DrawAxis( float fScale, glm::vec3 translate = glm::vec3(0) )
{
	glPushAttrib( GL_ENABLE_BIT );

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	glPushMatrix();
	glTranslatef( translate.x, translate.y, translate.z );
	glScalef( fScale, fScale, fScale );

	glBegin( GL_LINES );
	{
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0 );
		glVertex3f( 1.0f, 0.0f, 0.0f );

		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 1.0f, 0.0f );

		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 1.0f );
	}
	glEnd();

	glPopMatrix();
	glPopAttrib();
}

void OnDisplay()
{
 	switch(g_CurrentFX)
 	{
 	case 0: //Point light
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Color", glm::value_ptr(g_Light.m_Color));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Position", glm::value_ptr(g_Light.m_Position));
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.ConstantAttenuation", g_Light.m_ConstantAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.LinearAttenuation", g_Light.m_LinearAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.QuadraticAttenuation", g_Light.m_QuadraticAttenuation);
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			//DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 1: // Directional Light
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			glm::vec4 dir = glm::vec4(0.0f, 0.5f, 1.0f, 0.0f);
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Color", glm::value_ptr(g_Light.m_Color));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Direction", glm::value_ptr(dir));
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			//DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 2: // spotlight
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			glm::vec3 lightpos = glm::vec3(g_Light.m_Position.x, g_Light.m_Position.y, g_Light.m_Position.z);
 			g_LightViewMatrix = glm::lookAt(lightpos, glm::vec3(0), glm::vec3(0, -1, 0));
 			g_TextureMatrix = g_TextureBiasMatrix * g_LightProjectionMatrix * g_LightViewMatrix;
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Color", glm::value_ptr(g_Light.m_Color));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Position", glm::value_ptr(g_Light.m_Position));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Direction", glm::value_ptr(g_Light.m_Direction));
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.ConstantAttenuation", g_Light.m_ConstantAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.LinearAttenuation", g_Light.m_LinearAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.QuadraticAttenuation", g_Light.m_QuadraticAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.InnerCosAngle", g_Light.m_InnerCosAngle);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.OuterCosAngle", g_Light.m_OuterCosAngle);
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			//glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			//DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 3: // reflection
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Reflection", g_Material.m_Reflection);
 
 			g_FXList[g_CurrentFX]->SetParameterTexture("TEXUNIT5", g_EnvCubeMap);
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 4: // refraction
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Refraction", g_Material.m_Refraction);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Transmittance", g_Material.m_Transmittance);
 
 			g_FXList[g_CurrentFX]->SetParameterTexture("TEXUNIT5", g_EnvCubeMap);
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 5: // reflection + refraction
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Reflection", g_Material.m_Reflection);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Refraction", g_Material.m_Refraction);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Transmittance", g_Material.m_Transmittance);
 
 			g_FXList[g_CurrentFX]->SetParameterTexture("TEXUNIT5", g_EnvCubeMap);
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 6: // fresnel + chromatic dispersion
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Reflection", g_Material.m_Reflection);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Refraction", g_Material.m_Refraction);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Transmittance", g_Material.m_Transmittance);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Fresnelscale", g_Material.m_FresnelScale);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Fresnelpower", g_Material.m_FresnelPower);
 			g_FXList[g_CurrentFX]->SetParameter1f("Material.Fresnelbias", g_Material.m_FresnelBias);
 			g_FXList[g_CurrentFX]->SetParameter3f("Material.ETARatio", glm::value_ptr(g_Material.m_ETARatio));
 
 			g_FXList[g_CurrentFX]->SetParameterTexture("TEXUNIT5", g_EnvCubeMap);
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 7: // Normalmapping
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 
 			g_FXList[g_CurrentFX]->SetParameter1i("Light.LightType", (int)g_Light.m_Type);
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Color", glm::value_ptr(g_Light.m_Color));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Position", glm::value_ptr(g_Light.m_Position));
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.ConstantAttenuation", g_Light.m_ConstantAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.LinearAttenuation", g_Light.m_LinearAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.QuadraticAttenuation", g_Light.m_QuadraticAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.InnerCosAngle", g_Light.m_InnerCosAngle);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.OuterCosAngle", g_Light.m_OuterCosAngle);
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			//DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 8: // Projective Textures
 		{
 			glEnable( GL_DEPTH_TEST ); CheckGL();
 			glEnable( GL_CULL_FACE ); CheckGL();
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 
 			glm::vec3 lightpos = glm::vec3(g_Light.m_Position.x, g_Light.m_Position.y, g_Light.m_Position.z);
 			g_LightViewMatrix = glm::lookAt(lightpos, glm::vec3(0), glm::vec3(0, -1, 0));
 			g_TextureMatrix = g_TextureBiasMatrix * g_LightProjectionMatrix * g_LightViewMatrix;
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 			glm::mat4 invModelMatrix = glm::inverse(g_ModelMatrix);
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Color", glm::value_ptr(g_Light.m_Color));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Position", glm::value_ptr(invModelMatrix * g_Light.m_Position));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Direction", glm::value_ptr(glm::normalize(invModelMatrix * -g_Light.m_Direction)));
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.ConstantAttenuation", g_Light.m_ConstantAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.LinearAttenuation", g_Light.m_LinearAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.QuadraticAttenuation", g_Light.m_QuadraticAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.InnerCosAngle", g_Light.m_InnerCosAngle);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.OuterCosAngle", g_Light.m_OuterCosAngle);
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("TextureMatrix", glm::value_ptr(g_TextureMatrix * g_ModelMatrix));
 
 			g_FXList[g_CurrentFX]->SetParameterTexture("TEXUNIT4", g_ProjTexture);
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			//DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	case 9: // shadowmapping
 		{
 			glm::vec3 lightpos = glm::vec3(g_Light.m_Position.x, g_Light.m_Position.y, g_Light.m_Position.z);
 			g_LightViewMatrix = glm::lookAt(lightpos, glm::vec3(0), glm::vec3(0, -1, 0));
 			g_TextureMatrix = g_TextureBiasMatrix * g_LightProjectionMatrix * g_LightViewMatrix;
 
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 
 			glm::mat4 cameraMatrix = glm::inverse( g_Camera.GetViewMatrix() );
 
 			g_FXList[g_CurrentFX]->SetParameter4f("EyePosition", glm::value_ptr( cameraMatrix[3]));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Color", glm::value_ptr(g_Light.m_Color));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Position", glm::value_ptr(g_Light.m_Position));
 			g_FXList[g_CurrentFX]->SetParameter4f("Light.Direction", glm::value_ptr(g_Light.m_Direction));
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.ConstantAttenuation", g_Light.m_ConstantAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.LinearAttenuation", g_Light.m_LinearAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.QuadraticAttenuation", g_Light.m_QuadraticAttenuation);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.InnerCosAngle", g_Light.m_InnerCosAngle);
 			g_FXList[g_CurrentFX]->SetParameter1f("Light.OuterCosAngle", g_Light.m_OuterCosAngle);
 
 			glm::mat4 modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 			glm::mat3 modelMatrixIT = glm::transpose( glm::inverse( glm::mat3(g_ModelMatrix) ) );
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrix", glm::value_ptr(g_ModelMatrix));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelMatrixIT", glm::value_ptr(modelMatrixIT));
 			g_FXList[g_CurrentFX]->SetParameterMatrix("TextureMatrix", glm::value_ptr(g_TextureMatrix * g_ModelMatrix));
 			
 			g_FXList[g_CurrentFX]->SetParameterTexture("TEXUNIT6", g_DepthBufferID);
 
 			glMatrixMode( GL_MODELVIEW ); CheckGL();
 			glLoadMatrixf( glm::value_ptr(g_ViewMatrix) ); CheckGL();
 			//DrawCubeMap(g_EnvCubeMap);
 			//DrawAxis(2.0f, g_Camera.GetPivot());
 
 			g_MeshList[g_CurrentMesh]->SetEffect(g_FXList[g_CurrentFX]);
 
  			glBindFramebuffer(GL_FRAMEBUFFER, g_ShadowFBO);
  			glPushAttrib(GL_VIEWPORT_BIT);
  			glViewport(0, 0, g_ShadowBufferWidth, g_ShadowBufferHeight);
  			glClear(GL_DEPTH_BUFFER_BIT);
  
  			g_ViewMatrix = g_LightViewMatrix;
  			g_ProjectionMatrix = g_LightProjectionMatrix;
  
  			modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
  
  			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
  
 			CGtechnique tech = g_FXList[g_CurrentFX]->GetTechnique("shadow");
 
  			g_FXList[g_CurrentFX]->SetCurrentTechnique(tech);
  			g_MeshList[g_CurrentMesh]->Render();
  			glPopAttrib();
  			glBindFramebuffer(GL_FRAMEBUFFER, 0);
 			
 			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CheckGL();
 			g_ViewMatrix = g_Camera.GetViewMatrix();
 			g_ProjectionMatrix = g_Camera.GetProjectionMatrix();
 
 			modelViewProjectionMatrix = g_ProjectionMatrix * g_ViewMatrix * g_ModelMatrix;
 
 			g_FXList[g_CurrentFX]->SetParameterMatrix("ModelViewProjection", glm::value_ptr(modelViewProjectionMatrix));
 
 			g_FXList[g_CurrentFX]->SetCurrentTechnique(g_FXList[g_CurrentFX]->GetTechnique("main"));
 			g_MeshList[g_CurrentMesh]->Render();
 		}
 		break;
 	}

	glutSwapBuffers();
}

void OnClose()
{
	for(int i = 0; i < g_FXList.size(); i++)
		delete g_FXList[i];
}

void OnKeyReleased( unsigned char key, int x, int y )
{

}

void OnSpecialKeyPressed( int key, int x, int y )
{
	if(key == GLUT_KEY_F5)
	{
		g_FXList[g_CurrentFX]->Reload();
	}
}

void OnSpecialKeyReleased( int key, int x, int y )
{

}

void OnMouseButton( int button, int state, int x, int y )
{
	switch ( button )
	{
	case GLUT_LEFT_BUTTON:
		{
			g_bLeftMouseDown = ( state == GLUT_DOWN );
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		{
			g_bMiddleMouseDown = ( state == GLUT_DOWN );
		}
		break;
	case GLUT_RIGHT_BUTTON:
		{
			g_bRightMouseDown = ( state == GLUT_DOWN );
		}
		break;
	default: break;
	}

	g_PreviousMousePosition = glm::ivec2( x, y );

	g_Camera.MouseClicked( g_PreviousMousePosition );
}

void OnMouseWheel( int button, int state, int x, int y )
{
	// Note: state is 1 if scrolling is "up" and -1 if scrolling is "down"
	g_Camera.TranslateZ( state * -1.0f );

	// Don't allow zooming through the origin
	glm::vec3 cameraTranslate = g_Camera.GetTranslation();
	cameraTranslate.z = std::max( 0.5f, cameraTranslate.z );
	g_Camera.SetTranslate( cameraTranslate );

}

void OnMouseMotion( int x, int y )
{
	glm::ivec2 currentMousePosition( x, y );
	glm::ivec2 relativeMousePosition = currentMousePosition - g_PreviousMousePosition;

	// Check to see if any modifier keys are pressed.
	int modifiers = glutGetModifiers();
	bool bAltDown = ( modifiers & GLUT_ACTIVE_ALT );
	bool bCtrlDown = ( modifiers & GLUT_ACTIVE_CTRL );

	if ( g_bLeftMouseDown && bAltDown )
	{
		g_Camera.MouseMoved( currentMousePosition );
	}
	else if ( g_bLeftMouseDown && bCtrlDown )
	{
		g_Camera.TranslateZ( -relativeMousePosition.y * 0.2f );
	}
	else if ( g_bLeftMouseDown )
	{
		g_Camera.TranslatePivotX( -relativeMousePosition.x );
		g_Camera.TranslatePivotY( relativeMousePosition.y );
	}

	// Don't allow zooming through the origin
	glm::vec3 cameraTranslate = g_Camera.GetTranslation();
	cameraTranslate.z = std::max( 0.5f, cameraTranslate.z );
	g_Camera.SetTranslate( cameraTranslate );

	g_PreviousMousePosition = currentMousePosition;
}

void OnPassiveMouseMotion( int x, int y )
{

}

void DrawCubeMap( GLuint texID )
{
	glPushMatrix();

	glLoadIdentity();

	glm::vec3 eularAngles = g_Camera.GetEularAngles();

	glm::mat4 view_mat = glm::mat4( glm::mat3(g_Camera.GetViewMatrix() ) ); 
	glLoadMatrixf( glm::value_ptr( view_mat ) );

	glDepthMask( GL_FALSE );  // Don't write to the depth buffer
	glEnable( GL_TEXTURE_CUBE_MAP );
	glBindTexture( GL_TEXTURE_CUBE_MAP, texID );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); // Don't do any blending on the cube map textures

	glBegin( GL_QUADS );
	{
		// +X
		glTexCoord3f(  1, -1,  1 );
		glVertex3f(    1, -1,  1 );

		glTexCoord3f(  1,  1,  1 );
		glVertex3f(    1,  1,  1 );

		glTexCoord3f(  1,  1, -1 );
		glVertex3f(    1,  1, -1 );

		glTexCoord3f(  1, -1, -1 );
		glVertex3f(    1, -1, -1 );

		// -X
		glTexCoord3f( -1, -1, -1 );
		glVertex3f(   -1, -1, -1 );

		glTexCoord3f( -1,  1, -1 );
		glVertex3f(   -1,  1, -1 );

		glTexCoord3f( -1,  1,  1 );
		glVertex3f(   -1,  1,  1 );

		glTexCoord3f( -1, -1,  1 );
		glVertex3f(   -1, -1,  1 );

		// +Y
		glTexCoord3f( -1,  1, -1 );
		glVertex3f(   -1,  1, -1 );

		glTexCoord3f(  1,  1, -1 );
		glVertex3f(    1,  1, -1 );

		glTexCoord3f(  1,  1,  1 );
		glVertex3f(    1,  1,  1 );

		glTexCoord3f( -1, 1,  1 );
		glVertex3f(   -1, 1,  1 );

		// -Y
		glTexCoord3f( -1, -1,  1 );
		glVertex3f(   -1, -1,  1 );

		glTexCoord3f(  1, -1,  1 );
		glVertex3f(    1, -1,  1 );

		glTexCoord3f(  1, -1, -1 );
		glVertex3f(    1, -1, -1 );

		glTexCoord3f( -1, -1, -1 );
		glVertex3f(   -1, -1, -1 );		

		// +Z
		glTexCoord3f( -1,  1,  1 );
		glVertex3f(   -1,  1,  1 );

		glTexCoord3f(  1,  1,  1 );
		glVertex3f(    1,  1,  1 );

		glTexCoord3f(  1, -1,  1 );
		glVertex3f(    1, -1,  1 );

		glTexCoord3f( -1, -1,  1 );
		glVertex3f(   -1, -1,  1 );

		// -Z
		glTexCoord3f( -1, -1, -1 );
		glVertex3f(   -1, -1, -1 );

		glTexCoord3f(  1, -1, -1 );
		glVertex3f(    1, -1, -1 );

		glTexCoord3f(  1,  1, -1 );
		glVertex3f(    1,  1, -1 );

		glTexCoord3f( -1,  1, -1 );
		glVertex3f(   -1,  1, -1 );

		glEnd();
	}

	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	glDisable( GL_TEXTURE_CUBE_MAP );
	glDepthMask( GL_TRUE );

	glPopMatrix();
}