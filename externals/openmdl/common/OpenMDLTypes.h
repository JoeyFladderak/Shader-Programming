/*************************************************************************************************
Copyright 2010 OpenMDL.org. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

	1. 	Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.

	2. 	The origin of this software must not be misrepresented; you must 
		not claim that you wrote the original software.  If you use this 
		software in a product, an acknowledgment in the product 
		documentation is required.

	3. 	Altered source and binary versions must be plainly marked as such, and must
		not be misrepresented as being the original software.
	   
	4.	Altered file formats/specifications based on OpenMDL but not conforming the
		OpenMDL standard must be clearly marked as such, and must not be misrepresented
		as being the standard OpenMDL File Specification

	5. 	The name of the author may not be used to endorse or promote 
		products derived from this software without specific prior written 
		permission.
		
	6.	Attribution must be given in one of the following manners
		COMMERCIAL USAGE:
			a. 	one of the attribution logos provided with the OpenMDL SDK must be included 
				in a splash screen/startup sequence of your product with a minimum exposure 
				to the user of 3 seconds;
				AND
			b.	one of the attribution logos provided with the OpenMDL SDK must be clearly 
				printed on the box cover of your product;
				OR IF NOT APPLICABLE (i.e. digital distribution, etc.)
			c.	one of the attribution logos provided with the OpenMDL SDK must be 
				included on your product's website.
				
		NON-COMMERCIAL USAGE (demo reel, portfolio, hobby project, educational coursework assignment, etc.):
			d.	one of the attribution logos provided with the OpenMDL SDK, or the URL http://www.openmdl.org
				must be visible to the viewer/user in your project's documentation. Display of the 
				attribution logos in a demo reel video and/or screenshots is appreciated, but not required.
				
	7.	OpenMDL's File Specification may be changed as long as your customized specification is ONLY used 
		for your product.	

THIS SOFTWARE IS PROVIDED BY OPENMDL.ORG (ERIC H. DEN BOER) "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL OPENMDL.ORG (ERIC H. DEN BOER) OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of OpenMDL.org.

For a License FAQ, please go to http://www.openmdl.org/?page=license
*************************************************************************************************/
#ifndef _OPENMDL_COMMON_TYPES_H_
#define _OPENMDL_COMMON_TYPES_H_

namespace OpenMDL
{
	class Node;
	class Material;
	class Weight;
	class Mesh;
	class Camera;
	class CameraPose;
	class Light;
	class Joint;
	class JointPose;
	class Animation;
	class AnimationFrame;
	class Skeleton;
	class Transform;
	class Attribute;
	class Scene;
	class FaceGroup;
	class VertexPose;

	struct OPENMDL_EXPORT_FUNCTION float2
	{
		float2() { x = 0; y = 0; }
		float2(float _x, float _y) { x = _x; y = _y; }
		
		union
		{
			float cell[2];
			struct
			{
				float x;
				float y;
			};

			struct  
			{
				float u;
				float v;
			};
		};
	};

	struct OPENMDL_EXPORT_FUNCTION float3
	{
		float3() { x = 0; y = 0; z = 0; }
		float3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

		union
		{
			float cell[3];
			struct
			{
				float x;
				float y;
				float z;
			};
		};
	};

	struct OPENMDL_EXPORT_FUNCTION float4
	{
		float4() { x = 0; y = 0; z = 0; w = 0; }
		float4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

		union
		{
			float cell[4];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};
	};

	struct OPENMDL_EXPORT_FUNCTION matrix3x3
	{
		matrix3x3() 
		{ 
			c11 = 1; c12 = 0; c13 = 0;
			c21 = 0; c22 = 1; c23 = 0;
			c31 = 0; c32 = 0; c33 = 1;
		}

		union
		{
			float cell[9];
			float c[3][3];
			struct  
			{
				float	c11, c12, c13,
						c21, c22, c23,
						c31, c32, c33;
			};
		};
	};

	struct OPENMDL_EXPORT_FUNCTION matrix4x4
	{
		matrix4x4() 
		{ 
			c11 = 1; c12 = 0; c13 = 0; c14 = 0;
			c21 = 0; c22 = 1; c23 = 0; c24 = 0;
			c31 = 0; c32 = 0; c33 = 1; c34 = 0;
			c41 = 0; c42 = 0; c43 = 0; c44 = 1;
		}

		union
		{
			float cell[16];
			float c[4][4];
			struct  
			{
				float	c11, c12, c13, c14,
						c21, c22, c23, c24,
						c31, c32, c33, c34,
						c41, c42, c43, c44;
			};
		};
	};

	struct OPENMDL_EXPORT_FUNCTION colorf
	{
		colorf() { r = 0; g = 0; b = 0; a = 1; }
		colorf(float _r, float _g, float _b, float _a) { r = _r; g = _g; b = _b; a = _a; }

		bool IsBlack() { return !((r + g + b) > 0); }

		union
		{
			float cell[4];
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};
		};
	};

	enum LightType
	{
		eLightType_Ambient		= 1,
		eLightType_Directional	= 2,
		eLightType_Spot			= 4,
		eLightType_Point		= 8
	};

	enum DecayRate
	{
		eDecayRate_None			= 1,
		eDecayRate_Linear		= 2,
		eDecayRate_Quadratic	= 4,
		eDecayRate_Cubic		= 8
	};

	enum NodeType
	{
		eNodeType_Transform		= 1,
		eNodeType_Mesh			= 2,
		eNodeType_Joint			= 4,
		eNodeType_Light			= 8,
		eNodeType_Camera		= 16
	};

	enum AttributeType
	{
		eAttributeType_Int		= 1,
		eAttributeType_Float	= 2,
		eAttributeType_String	= 4,
		eAttributeType_Float3	= 8
	};

	enum FaceType
	{
		eFaceType_Triangle		= 1,
		eFaceType_Quad			= 2
	};

	typedef OpenMDL::Vector<Node*>					NodeList;
	typedef OpenMDL::Vector<Material*>				MaterialList;
	typedef OpenMDL::Vector<Mesh*>					MeshList;
	typedef OpenMDL::Vector<Camera*>				CameraList;
	typedef OpenMDL::Vector<Light*>					LightList;
	typedef OpenMDL::Vector<Joint*>					JointList;
	typedef OpenMDL::Vector<Animation*>				AnimationList;
	typedef OpenMDL::Vector<Skeleton*>				SkeletonList;
	typedef OpenMDL::Vector<Weight*>				WeightList;
	typedef OpenMDL::Vector<Transform*>				TransformList;
	typedef OpenMDL::Vector<Attribute*>				AttributeList;
	typedef OpenMDL::Vector<float2>					Float2List;
	typedef OpenMDL::Vector<float3>					Float3List;
	typedef OpenMDL::Vector<float4>					Float4List;
	typedef OpenMDL::Vector<Float2List>				UVSetList;
	typedef OpenMDL::Vector<int>					IntList;
	typedef OpenMDL::Vector<unsigned int>			UIntList;
	typedef OpenMDL::Vector<OpenMDL::Vector<unsigned int>>	UVIndexSetList;
	typedef OpenMDL::Vector<String>					StringList;
	typedef OpenMDL::Vector<Scene*>					SceneList;
	typedef OpenMDL::Vector<FaceGroup*>				FaceGroupList;
	typedef OpenMDL::Vector<AnimationFrame*>		FrameList;
	typedef OpenMDL::Vector<JointPose*>				JointPoseList;
	typedef OpenMDL::Vector<CameraPose*>			CameraPoseList;
	typedef OpenMDL::Vector<VertexPose*>			VertexPoseList;
}

#endif