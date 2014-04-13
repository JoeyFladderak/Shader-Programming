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
#ifndef _OPENMDL_COMMON_CAMERA_H_
#define _OPENMDL_COMMON_CAMERA_H_

#include "OpenMDLcommon.h"
#include "OpenMDLNode.h"

namespace OpenMDL
{
	class OPENMDL_EXPORT_FUNCTION Camera : public Node
	{
	public:
		Camera();
		Camera(const String& name, const matrix4x4& transform, float aspectratio, float znear, float zfar, float hfov, float vfov);
		virtual ~Camera();

		float		GetAspectRatio() const { return m_AspectRatio; }
		float		GetZNear() const { return m_ZNear; }
		float		GetZFar() const { return m_ZFar; }
		float		GetHFov() const { return m_HFov; }
		float		GetVFov() const { return m_VFov; }

		void		SetAspectRatio(float aspectratio) { m_AspectRatio = aspectratio; }
		void		SetZNear(float znear) { m_ZNear = znear; }
		void		SetZFar(float zfar) { m_ZFar = zfar; }
		void		SetHFov(float hfov) { m_HFov = hfov; }
		void		SetVFov(float vfov) { m_VFov = vfov; }

	protected:
		float		m_AspectRatio;
		float		m_ZNear;
		float		m_ZFar;
		float		m_HFov;
		float		m_VFov;
	};
}

#endif