#include "box3d/collision.h"
#include "box3d/id.h"
#include "box3d/math_functions.h"
#include "box3d/types.h"

#include <box3d/box3d.h>
#include <courage/mjcf.h>
#include <mujoco/mjspec.h>
#include <mujoco/mjtype.h>
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

	mjsElement* curr_object_elm = mjs_firstChild( worldbody, mjOBJ_BODY, 0 );
	while ( curr_object_elm )
	{
		mjsBody* curr_root_body = mjs_asBody( curr_object_elm );
		if ( !curr_root_body )
		{
			break;
		}

		mjString* name_mjstr = mjs_getName( curr_object_elm );
		const char* name_str = name_mjstr != NULL ? mjs_getString( name_mjstr ) : "";
		if ( mjcfIsArticulated( curr_root_body ) )
		{
			printf( "[INFO] Articulated obj: %s\n", name_str );
		}
		else
		{
			printf( "[INFO] Non-Articulated obj: %s\n", name_str );
			mjcfLoadNonArticulated( worldId, curr_root_body );
		}

		curr_object_elm = mjs_nextChild( worldbody, curr_object_elm, 0 );
	}

	return true;
}

// Internal helper functions ----------------------------------------------------------------------

int mjcfGetNumObjects( mjSpec* spec )
{
	mjsBody* worldbody = mjs_findBody( spec, "world" );
	if ( !worldbody )
	{
		return 0;
	}

	int n_bodies = 0;

	mjsElement* curr_body_elm = mjs_firstChild( worldbody, mjOBJ_BODY, 0 );
	while ( curr_body_elm )
	{
		mjsBody* curr_body = mjs_asBody( curr_body_elm );
		if ( !curr_body )
		{
			break;
		}

		mjString* name = mjs_getName( curr_body_elm );
		const char* name_str = mjs_getString( name );
		printf( "Body: %s\n", name_str );
		n_bodies++;

		curr_body_elm = mjs_nextChild( worldbody, curr_body_elm, 0 );
	}

	return n_bodies;
}

bool mjcfIsArticulated( mjsBody* body )
{
	mjsElement* curr_jnt_elm = mjs_firstChild( body, mjOBJ_JOINT, 1 );
	while ( curr_jnt_elm )
	{
		mjsJoint* curr_jnt = mjs_asJoint( curr_jnt_elm );
		if ( curr_jnt->type != mjJNT_FREE )
		{
			return true;
		}

		curr_jnt_elm = mjs_nextChild( body, curr_jnt_elm, 1 );
	}

	return false;
}

void mjcfLoadNonArticulated( b3WorldId worldId, mjsBody* rootBody )
{
	bool isDynamic = false;
	{
		mjsElement* free_jnt_elm = mjs_firstChild( rootBody, mjOBJ_JOINT, 0 );
		if ( free_jnt_elm )
		{
			mjsJoint* free_jnt = mjs_asJoint( free_jnt_elm );
			if ( free_jnt->type == mjJNT_FREE )
			{
				isDynamic = true;
			}
		}
	}

	if ( isDynamic )
	{
		b3BodyDef bodyDef = b3DefaultBodyDef();
		bodyDef.type = b3_dynamicBody;
		bodyDef.position = (b3Pos){ rootBody->pos[0], rootBody->pos[1], rootBody->pos[2] };
		bodyDef.rotation.s = rootBody->quat[0];
		bodyDef.rotation.v = (b3Vec3){ rootBody->quat[1], rootBody->quat[2], rootBody->quat[3] };

		// TODO(wilbert): have to double check if the body has at least 1 geom in the whole tree

		b3BodyId bodyId = b3CreateBody( worldId, &bodyDef );

		mjsBody* curr_body = rootBody;
		b3Transform curr_body_to_root_tf = { { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0, 0.0 } };

		mjsElement* curr_geom_elm = mjs_firstChild( curr_body, mjOBJ_GEOM, 0 );

		while ( curr_geom_elm )
		{
			mjsGeom* curr_geom = mjs_asGeom( curr_geom_elm );

			b3Transform local_geom_tf = mjcfTransformFromPQ( curr_geom->pos, curr_geom->quat );

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = curr_geom->density;
			shapeDef.baseMaterial.friction = curr_geom->friction[0];

			switch ( curr_geom->type )
			{
				case mjGEOM_BOX:
				{
					b3BoxHull dynamicBox = b3MakeTransformedBoxHull(
						curr_geom->size[0], curr_geom->size[1], curr_geom->size[2], local_geom_tf );
					b3CreateHullShape( bodyId, &shapeDef, &dynamicBox.base );
					break;
				}

				case mjGEOM_SPHERE:
				{
					b3Sphere dynamicSphere = { local_geom_tf.p, curr_geom->size[0] };
					b3CreateSphereShape( bodyId, &shapeDef, &dynamicSphere );
					break;
				}

				case mjGEOM_CYLINDER:
				{
					b3HullData* cylinder =
						b3CreateCylinder( curr_geom->size[1] * 2.0, curr_geom->size[0], 0.0, 32 );

					// TODO(wilbert): apply the local transform for this geom

					b3CreateHullShape( bodyId, &shapeDef, cylinder );
					break;
				}

				case mjGEOM_CAPSULE:
				{
					b3Capsule capsule;
					capsule.radius = curr_geom->size[1];
					capsule.center1 = (b3Vec3){ 0.0, 0.0, curr_geom->size[0] };
					capsule.center2 = (b3Vec3){ 0.0, 0.0, -curr_geom->size[0] };

					// TODO(wilbert): apply the local transform for this geom

					b3CreateCapsuleShape( bodyId, &shapeDef, &capsule );
					break;
				}

				case mjGEOM_ELLIPSOID:
				{

					break;
				}

				case mjGEOM_MESH:
				{

					break;
				}

				default:
				{

					break;
				}
			}

			curr_geom_elm = mjs_nextChild( curr_body, curr_geom_elm, 0 );
		}
	}
}

b3Transform mjcfTransformFromPQ( const mjtNum p[3], const mjtNum q[4] )
{
	b3Transform tf;
	tf.p = (b3Vec3){ p[0], p[1], p[2] };
	tf.q = (b3Quat){ { q[1], q[2], q[3] }, q[0] };
	return tf;
}
