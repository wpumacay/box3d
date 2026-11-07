#pragma once

#include <box3d/base.h>
#include <box3d/id.h>
#include <mujoco/mjspec.h>
#include <stdbool.h>

B3_API bool b3LoadMjcfString( b3WorldId worldId, const char* xml_string );
B3_API bool b3LoadMjcfFile( b3WorldId worldId, const char* filepath );
B3_API bool b3LoadMjcfSpec( b3WorldId worldId, mjSpec* spec );
