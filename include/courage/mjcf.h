#pragma once

#include <box3d/base.h>
#include <box3d/id.h>
#include <box3d/math_functions.h>
#include <mujoco/mjspec.h>
#include <stdbool.h>

B3_API bool b3LoadMjcfString( b3WorldId worldId, const char* xml_string );
B3_API bool b3LoadMjcfFile( b3WorldId worldId, const char* filepath );
B3_API bool b3LoadMjcfSpec( b3WorldId worldId, mjSpec* spec );

// Internal helper functions ----------------------------------------------------------------------

int mjcfGetNumObjects( mjSpec* spec );
bool mjcfIsArticulated( mjsBody* body );
void mjcfLoadNonArticulated( b3WorldId worldId, mjsBody* rootBody );
b3Transform mjcfTransformFromPQ( const mjtNum p[3], const mjtNum q[4] );
