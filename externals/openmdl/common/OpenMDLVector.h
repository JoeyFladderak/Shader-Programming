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
#ifndef _OPENMDL_COMMON_VECTOR_H_
#define _OPENMDL_COMMON_VECTOR_H_

#define OPENMDL_VECTOR_SIZE 4

namespace OpenMDL
{
	template <class _Type>
	class Vector
	{
	public:
		Vector();
		Vector(const Vector<_Type>& copy);
		~Vector();
		
		void					Push(const _Type& item);
		void					Pop();
		unsigned int			Size() const { return (unsigned int)m_Top + 1; }
		const _Type&			At(int index) const { return m_Buffer[index]; }
		_Type&					At(int index) { return m_Buffer[index]; }
		void					Clear(bool deallocate = false);
		void					Erase(int index);
		void					Deallocate();
		void					Allocate(int elements);
		unsigned int			Capacity() const { return (unsigned int)m_BufferSize; }
		const _Type*			GetBuffer() const { return m_Buffer; }

		const _Type& operator	[] (int index) const { return m_Buffer[index]; }
		_Type& operator			[] (int index) { return m_Buffer[index]; }
		void operator			= (const Vector<_Type>& copy);

	protected:
		void					clearMemory();

		int						m_Top;
		_Type*					m_Buffer;
		int						m_BufferSize;
	};

	//////////////////////////////////////////////////////////////////////////
	// Vector<_Type> implementations
	//////////////////////////////////////////////////////////////////////////

	template <class _Type>
	OpenMDL::Vector<_Type>::Vector()
	{
		m_Top = -1;
		m_Buffer = new _Type[OPENMDL_VECTOR_SIZE];
		m_BufferSize = OPENMDL_VECTOR_SIZE;

		clearMemory();
	}

	template <class _Type>
	OpenMDL::Vector<_Type>::Vector(const Vector<_Type>& copy)
	{
		m_Top = copy.m_Top;
		m_Buffer = new _Type[copy.m_BufferSize];
		m_BufferSize = copy.m_BufferSize;
		memcpy(m_Buffer, copy.m_Buffer, sizeof(_Type) * m_BufferSize);
	}

	template <class _Type>
	OpenMDL::Vector<_Type>::~Vector()
	{
		OPENMDL_SAFE_ARR_DELETE(m_Buffer);
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::Push( const _Type& item )
	{
		if((m_Top + 1) >= m_BufferSize)
		{
			const int newBufferSize = m_BufferSize + (m_BufferSize >> 1);

			// allocate new buffer
			_Type* tempbuffer = new _Type[newBufferSize];
			memcpy(tempbuffer, m_Buffer, sizeof(_Type) * m_BufferSize);

			// delete old buffer and reference the new data and size
			OPENMDL_SAFE_ARR_DELETE(m_Buffer);
			m_Buffer = tempbuffer;
			m_BufferSize = newBufferSize;
		}

		++m_Top;
		m_Buffer[m_Top] = item;
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::Pop()
	{
		if(m_Top < 0) 
			return;

		--m_Top;
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::operator=( const Vector<_Type>& copy )
	{
		OPENMDL_SAFE_ARR_DELETE(m_Buffer);

		m_Top = copy.m_Top;
		m_Buffer = new _Type[copy.m_BufferSize];
		m_BufferSize = copy.m_BufferSize;
		memcpy(m_Buffer, copy.m_Buffer, sizeof(_Type) * m_BufferSize);
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::Clear( bool deallocate /* = false */ )
	{
		if(deallocate)
		{
			OPENMDL_SAFE_ARR_DELETE(m_Buffer);
			m_Buffer = new _Type[OPENMDL_VECTOR_SIZE];
			m_BufferSize = OPENMDL_VECTOR_SIZE;

			clearMemory();
		}

		m_Top = -1;
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::Erase( int index )
	{
		if(index >= 0 && index < m_Top)
		{		
			memcpy(&m_Buffer[index], &m_Buffer[index + 1], sizeof(_Type) * (m_Top - index));
			--m_Top;
		}
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::Deallocate()
	{
		OPENMDL_SAFE_ARR_DELETE(m_Buffer);
		m_Buffer = new _Type[OPENMDL_VECTOR_SIZE];
		m_BufferSize = OPENMDL_VECTOR_SIZE;
		m_Top = -1;

		clearMemory();
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::Allocate( int elements )
	{
		OPENMDL_SAFE_ARR_DELETE(m_Buffer);
		m_Buffer = new _Type[elements];
		m_BufferSize = elements;
		m_Top = -1;

		clearMemory();
	}

	template <class _Type>
	void OpenMDL::Vector<_Type>::clearMemory()
	{
		// make sure to only clear pointer buffers
		// yes this is absolutely postively horrible and i don't wanna hear about it! :)
		unsigned int addressSize = sizeof(void*);
		unsigned int typeSize = sizeof(_Type);

		if(typeSize == 4 && addressSize == 4)
		{
			memset(m_Buffer, 0, 4 * m_BufferSize);
		}
		else if(typeSize == 8 && addressSize == 8)
		{
			memset(m_Buffer, 0, 8 * m_BufferSize);
		}
	}
}

#endif