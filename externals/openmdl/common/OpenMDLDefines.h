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
#ifndef _OPENMDL_COMMON_DEFINES_H_
#define _OPENMDL_COMMON_DEFINES_H_

// compiler configuration

#ifdef _MSC_VER
#	define OPENMDL_VISUALSTUDIO
#	define OPENMDL_COMPILER					"MSVC"
#	define OPENMDL_FUNCTION					__FUNCTION__
#	define OPENMDL_FILE						__FILE__
#	define OPENMDL_LINE						__LINE__
#	define OPENMDL_INLINE					__inline
#	define OPENMDL_LIBEXPORT				__declspec(dllexport)
#	define OPENMDL_LIBIMPORT				__declspec(dllimport)

#	pragma warning( disable : 4275 )
#	pragma warning( disable : 4996 )
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4482 )

#	ifdef _WIN32
#		define OPENMDL_ENV32
#	endif

#	ifdef _WIN64
#		define OPENMDL_ENV64
#	endif
#endif

#ifdef __GNUC__
#	define OPENMDL_GCC
#	define OPENMDL_COMPILER					"GCC"
#	define OPENMDL_FUNCTION					__func__
#	define OPENMDL_FILE						__FILE__
#	define OPENMDL_LINE						__LINE__
#	define OPENMDL_INLINE					inline

#	if __x86_64__ || __ppc64__
#		define OPENMDL_ENV64
#	else
#		define OPENMDL_ENV32
#	endif
#endif



// libexp define

#ifdef OPENMDL_LIBEXP
#	define OPENMDL_EXPORT_FUNCTION			OPENMDL_LIBEXPORT
#else
#	define OPENMDL_EXPORT_FUNCTION			OPENMDL_LIBIMPORT
#endif



// platform configuration

#ifdef _WIN32
#	define OPENMDL_WINDOWS
#endif

#ifdef _WIN64
#	define OPENMDL_WINDOWS
#endif

#ifdef __APPLE__
#	define OPENMDL_OSX
#endif

#ifdef __linux__
#	define OPENMDL_LINUX
#endif



// common helper defines and OpenMDL-specific defines

#define OPENMDL_ALIGNEDMALLOC(x)		_aligned_malloc(x, 64)
#define OPENMDL_ALIGNEDFREE(x)			_aligned_free(x)
#define OPENMDL_SAFE_DELETE(x)			{ if(x) delete x; x = NULL; }
#define OPENMDL_SAFE_ARR_DELETE(x)		{ if(x) delete [] x; x = NULL; }
#define OPENMDL_SAFE_RELEASE(x)			{ if(x) x->Release(); }
#define OPENMDL_SAFE_RELEASE_DELETE(x)	{ if(x) x->Release(); delete x; x = NULL; }
#define OPENMDL_SAFE_FCLOSE(x)			{ if(x) fclose(x); x = NULL; }
#define OPENMDL_SAFE_FREE(x)			{ if(x) free(x); }
#define OPENMDL_BIT(x)					(1 << (x))
#define OPENMDL_BOOL_TO_BYTE(b)			((b) ? 0xFF : 0x00)
#define OPENMDL_BYTE_TO_BOOL(b)			((b) ? true : false)
#define OPENMDL_VERSION_STRING			"0.96 beta"
#define OPENMDL_VERSION_INTEGER			960
#define OPENMDL_VERSION_DOUBLE			0.960
#define OPENMDL_VERSION_FLOAT			0.960f
#define OPENMDL_SCENE_HEADER_ID			0x304c444d6e65704f // Scene file header ID: "OpenMDL0"
#define OPENMDL_ANIMATION_HEADER_ID		0x334c444d6e65704f // Animation file header ID: "OpenMDL3"
#define OPENMDL_HEADER_OPTIONCOUNT		3
#define OPENMDL_STRLEN					128
#define OPENMDL_MAX_TEXCOORDSETS		32

#endif