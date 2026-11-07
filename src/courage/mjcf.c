#include <courage/mjcf.h>
#include <mujoco/mjspec.h>
#include <mujoco/mujoco.h>
#include <stdio.h>

#define ERROR_BUFFER_LEN 1000

bool b3LoadMjcfString( b3WorldId worldId, const char* xml_string )
{
	char error_buffer[ERROR_BUFFER_LEN];
	mjSpec* spec = mj_parseXMLString( xml_string, NULL, error_buffer, ERROR_BUFFER_LEN );

	if ( !spec )
	{
		printf( "[ERROR] Couldn't parse given string\n" );
		return false;
	}

	return b3LoadMjcfSpec( worldId, spec );
}

bool b3LoadMjcfFile( b3WorldId worldId, const char* filepath )
{
	char error_buffer[1000];
	mjSpec* spec = mj_parseXML( filepath, NULL, error_buffer, 1000 );

	if ( !spec )
	{
		printf( "[ERROR] Couldn't parse mjcf @ %s\n", filepath );
		return false;
	}

	return b3LoadMjcfSpec( worldId, spec );
}

bool b3LoadMjcfSpec( b3WorldId worldId, mjSpec* spec )
{
	mjsBody* worldbody = mjs_findBody( spec, "world" );
	if ( !worldbody )
	{
		printf( "[ERROR] Couldn't find <worldbody> T_T" );
		return false;
	}

	mjsElement* elm = mjs_firstChild( worldbody, mjOBJ_BODY, 0 );
	if ( elm )
	{
		mjsBody* body = mjs_asBody( elm );
		if ( body )
		{
			printf( "Body: %s\n", mjs_getName( elm ) );
		}
	}

	return true;
}
