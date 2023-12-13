#ifndef PROTO_OPEN_SINGLETON_H
#define PROTO_OPEN_SINGLETON_H

#pragma once

#include <assert.h>

//! Convenient macro for implementing a singleton
#define PROTO_SINGLETON_IMPL( Type ) template<> Type* cSingleton< Type >::s_pInstance = 0;

/*namespace proto
{*/

//! Inherit from this class for your singletons ( phoenix type, single threaded singleton )
template< class TypeT >
class cSingleton
{
    public:
		static inline TypeT* create()
		{
			assert( !s_pInstance && "the singleton has not yet been recreated" );
			s_pInstance = new TypeT;
			return s_pInstance;
		}

		static inline TypeT* instance()
		{
			assert( s_pInstance && "the singleton has not yet been created" );
			return s_pInstance;
		}

	   
		static inline void destroy()
		{
			assert( s_pInstance && "the singleton instance is invalid");

			if ( s_pInstance )
				delete s_pInstance;

			s_pInstance = 0;
		}

		static bool isCreated()
		{
			return (s_pInstance != NULL);
		}

	protected:
		cSingleton() {}
		virtual ~cSingleton()
		{
		}

	private:
		static TypeT* s_pInstance;
    
        //! Avoid copy constructor
        cSingleton( const cSingleton& );
 
        //! Avoid assignment operator
        cSingleton& operator = ( const cSingleton& );
};

/*}*/
#endif // SINGLETON_H