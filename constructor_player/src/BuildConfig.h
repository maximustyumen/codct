#ifndef BUILDCONFIG
#define BUILDCONFIG

#include <iostream>
#include <string>

#define NOMINMAX

//__DATE__ __TIME__ __cplusplus __STDC__

static std::string player_version = "MAXlab player v.3.2.0 64bit. Author: Maxim Gammer (MaxGammer@gmail.com)";

//PhysX
#ifdef WIN32
	#define PHYSX true
#endif


#define RTI_ON true
//#define RTI_THREAD true

#define JUMP_version true

#endif
