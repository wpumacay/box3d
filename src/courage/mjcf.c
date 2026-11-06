#include "mjcf.h"

#include <mujoco/mujoco.h>
#include <stdio.h>

bool load_mjcf( const char* filepath )
{
	char error_buffer[1000];
	mjSpec* spec = mj_parseXML( filepath, NULL, error_buffer, 1000 );

	if ( !spec )
	{
		printf( "[ERROR] Couldn't parse mjcf @ %s\n", filepath );
		return false;
	}

	return true;
}
