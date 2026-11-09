// SPDX-FileCopyrightText: 2025 Erin Catto
// SPDX-License-Identifier: MIT

#include "gfx/debug_adapter.h"
#include "gfx/draw.h"
#include "gfx/keycodes.h"
#include "imgui.h"
#include "sample.h"

#include "box3d/box3d.h"
#include "courage/mjcf.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

static const char* TEST_XML = R"(
<mujoco>
  <worldbody>
    <light diffuse=".5 .5 .5" pos="0 0 3" dir="0 0 -1"/>
    <geom type="plane" size="1 1 0.1" rgba=".9 0 0 1"/>
    <body name="cube" pos="1 0 1">
      <joint type="free"/>
      <geom type="box" size=".1 .2 .3" rgba="0 .9 0 1"/>
    </body>
    <body name="sphere" pos="2 0 1">
      <freejoint />
      <geom type="sphere" size=".15" rgba="0 .9 0 1"/>
    </body>
  </worldbody>
</mujoco>
)";

class LoaderMjcf : public Sample
{
public:
	explicit LoaderMjcf( SampleContext* context )
		: Sample( context )
	{
		if ( m_context->restart == false )
		{
			m_camera->SetView( 0.0f, 30.0f, 30.0f, { 0.0f, 0.0f, 1.5f } );
		}

		b3World_SetGravity( m_worldId, { 0.0, 0.0, -10.0 } );

		m_isEnabled = true;

		{
			b3BodyDef groundBodyDef = b3DefaultBodyDef();
			groundBodyDef.name = "ground";
			groundBodyDef.position = { 0.0f, 0.0f, -1.0f };
			b3BodyId groundId = b3CreateBody( m_worldId, &groundBodyDef );

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			b3BoxHull groundHull = b3MakeBoxHull( 20.0, 20.0, 1.0 );
			b3ShapeId shapeId = b3CreateHullShape( groundId, &shapeDef, &groundHull.base );

			SetGroundShape( shapeId );
		}

		if ( b3LoadMjcfString( m_worldId, TEST_XML ) )
		{
			printf( "Successfully loaded from xml test string" );
		}

		printf( "---------------------------------\n\n" );

		if ( auto env_var = std::getenv( "MOLMOSPACES_ASSETS_DIR" ) )
		{
			printf( "ENV_VAR: %s\n", env_var );
			fs::path assets_dir( env_var );
			fs::path scene_path = assets_dir / "scenes" / "ithor" / "FloorPlan1_physics.xml";
			if ( fs::exists( scene_path ) )
			{
				if ( b3LoadMjcfFile( m_worldId, scene_path.c_str() ) )
				{
					printf( "Successfully loaded from file %s\n", scene_path.c_str() );
				}
				else
				{
					printf( "[ERROR] Couldn't load file %s\n", scene_path.c_str() );
				}
			}
			else
			{
				printf( "[ERROR] Filepath @ %s doesn't exist\n", scene_path.c_str() );
			}
		}
	}

	bool DrawControls() override
	{
		return true;
	}

	void Step() override
	{
		Sample::Step();
	}

	static Sample* Create( SampleContext* context )
	{
		context->viewZUp = true;
		return new LoaderMjcf( context );
	}

	bool m_isEnabled;
};

static int sampleMujocoMjcf = RegisterSample( "Mujoco", "Loader mjcf", LoaderMjcf::Create );
