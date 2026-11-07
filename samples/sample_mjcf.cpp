// SPDX-FileCopyrightText: 2025 Erin Catto
// SPDX-License-Identifier: MIT

#include "gfx/draw.h"
#include "gfx/keycodes.h"
#include "imgui.h"
#include "sample.h"

#include "box3d/box3d.h"
#include "courage/mjcf.h"

#include <cstdio>

static const char* TEST_XML = R"(
<mujoco>
  <worldbody>
    <light diffuse=".5 .5 .5" pos="0 0 3" dir="0 0 -1"/>
    <geom type="plane" size="1 1 0.1" rgba=".9 0 0 1"/>
    <body name="cube" pos="0 0 1">
      <joint type="free"/>
      <geom type="box" size=".1 .2 .3" rgba="0 .9 0 1"/>
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
			m_camera->SetView( 0.0f, 30.0f, 30.0f, { 0.0f, 1.5f, 0.0f } );
		}

		m_isEnabled = true;

		b3BodyId groundId = AddGroundBox( 20.0f );

		if ( b3LoadMjcfString( m_worldId, TEST_XML ) )
		{
			printf( "Successfully loaded from xml test string" );
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
		return new LoaderMjcf( context );
	}

	bool m_isEnabled;
};

static int sampleMujocoMjcf = RegisterSample( "Mujoco", "Loader mjcf", LoaderMjcf::Create );
