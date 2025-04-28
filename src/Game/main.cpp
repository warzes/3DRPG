#include "stdafx.h"
#if ENABLE_SAMPLES
#include "s0001OGL3Example.h"
#include "s0002OGL3Example.h"
#include "s0003OGL3Example.h"
#include "s0004OGL3Example.h"
#include "s0005OGL3Example.h"
#include "s0006OGL3Example.h"
#include "s0007OGL3Example.h"
#include "s0008OGL3Example.h"
#include "s0009OGL3Example.h"
#include "s0010OGL3Example.h"

#include "s0001OGL4Example.h"
#else
#endif
//=============================================================================
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//=============================================================================
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
#if ENABLE_SAMPLES
	//SampleFrameworkOGL4* example = new s0001OGL4Example();
	//StartSampleOGL4(1600, 900, example);
	//delete example;


	SampleFrameworkOGL3* example = new 
		//s0001OGL3Example();
		//s0002OGL3Example();
		//s0003OGL3Example();
		//s0004OGL3Example();
		//s0005OGL3Example();
		//s0006OGL3Example();
		//s0007OGL3Example();
		//s0008OGL3Example();
		//s0009OGL3Example();
		s0010OGL3Example();

	StartSampleOGL3(1600, 900, example);
	delete example;


#else
	
#endif
}
//=============================================================================