// SPDX-FileCopyrightText: 2025 Erin Catto
// SPDX-License-Identifier: MIT

#include "gfx/draw.h"
#include "gfx/keycodes.h"
#include "imgui.h"
#include "sample.h"

#include "box3d/box3d.h"

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

		m_type = b3_dynamicBody;
		m_isEnabled = true;

		b3BodyId groundId = AddGroundBox( 20.0f );

		// Define attachment
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = b3_dynamicBody;
			bodyDef.position = { -2.0f, 3.0f, 0.0f };
			bodyDef.name = "attach1";
			m_attachmentId = b3CreateBody( m_worldId, &bodyDef );

			b3BoxHull box = b3MakeBoxHull( 0.5f, 2.0f, 0.5f );
			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 1.0f;
			b3CreateHullShape( m_attachmentId, &shapeDef, &box.base );
		}

		// Define second attachment
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = m_type;
			bodyDef.isEnabled = m_isEnabled;
			bodyDef.position = { 3.0f, 3.0f };
			bodyDef.name = "attach2";
			m_secondAttachmentId = b3CreateBody( m_worldId, &bodyDef );

			b3BoxHull box = b3MakeBoxHull( 0.5f, 2.0f, 0.5f );
			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 1.0f;
			b3CreateHullShape( m_secondAttachmentId, &shapeDef, &box.base );
		}

		// Define platform
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = m_type;
			bodyDef.isEnabled = m_isEnabled;
			bodyDef.position = { -4.0f, 5.0f };
			bodyDef.name = "platform";
			m_platformId = b3CreateBody( m_worldId, &bodyDef );

			b3BoxHull box = b3MakeTransformedBoxHull(
				0.5f, 4.0f, 0.5f,
				{ { 4.0f, 0.0f, 0.0f }, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, 0.5f * B3_PI ) } );

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 2.0f;
			b3CreateHullShape( m_platformId, &shapeDef, &box.base );

			b3RevoluteJointDef revoluteDef = b3DefaultRevoluteJointDef();
			b3Pos pivot = { -2.0f, 5.0f, 0.0f };
			revoluteDef.base.bodyIdA = m_attachmentId;
			revoluteDef.base.bodyIdB = m_platformId;
			revoluteDef.base.localFrameA.p = b3Body_GetLocalPoint( m_attachmentId, pivot );
			revoluteDef.base.localFrameB.p = b3Body_GetLocalPoint( m_platformId, pivot );
			revoluteDef.maxMotorTorque = 50.0f;
			revoluteDef.enableMotor = true;
			b3CreateRevoluteJoint( m_worldId, &revoluteDef );

			pivot = { 3.0f, 5.0f };
			revoluteDef.base.bodyIdA = m_secondAttachmentId;
			revoluteDef.base.bodyIdB = m_platformId;
			revoluteDef.base.localFrameA.p = b3Body_GetLocalPoint( m_secondAttachmentId, pivot );
			revoluteDef.base.localFrameB.p = b3Body_GetLocalPoint( m_platformId, pivot );
			revoluteDef.maxMotorTorque = 50.0f;
			revoluteDef.enableMotor = true;
			b3CreateRevoluteJoint( m_worldId, &revoluteDef );

			b3PrismaticJointDef prismaticDef = b3DefaultPrismaticJointDef();
			b3Pos anchor = { 0.0f, 5.0f, 0.0f };
			prismaticDef.base.bodyIdA = groundId;
			prismaticDef.base.bodyIdB = m_platformId;
			prismaticDef.base.localFrameA.p = b3Body_GetLocalPoint( groundId, anchor );
			prismaticDef.base.localFrameB.p = b3Body_GetLocalPoint( m_platformId, anchor );
			prismaticDef.maxMotorForce = 1000.0f;
			prismaticDef.motorSpeed = 0.0f;
			prismaticDef.enableMotor = true;
			prismaticDef.lowerTranslation = -10.0f;
			prismaticDef.upperTranslation = 10.0f;
			prismaticDef.enableLimit = true;

			b3CreatePrismaticJoint( m_worldId, &prismaticDef );

			m_speed = 3.0f;
		}

		// Create a payload
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = b3_dynamicBody;
			bodyDef.position = { -3.0f, 8.0f };
			bodyDef.name = "crate1";
			b3BodyId bodyId = b3CreateBody( m_worldId, &bodyDef );

			b3BoxHull box = b3MakeBoxHull( 0.75f, 0.75f, 0.75f );

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 2.0f;

			b3CreateHullShape( bodyId, &shapeDef, &box.base );
		}

		// Create a second payload
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = m_type;
			bodyDef.isEnabled = m_isEnabled;
			bodyDef.position = { 2.0f, 8.0f };
			bodyDef.name = "crate2";
			m_secondPayloadId = b3CreateBody( m_worldId, &bodyDef );

			b3BoxHull box = b3MakeBoxHull( 0.75f, 0.75f, 0.75f );

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 2.0f;

			b3CreateHullShape( m_secondPayloadId, &shapeDef, &box.base );
		}

		// Create a separate body on the ground
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = m_type;
			bodyDef.isEnabled = m_isEnabled;
			bodyDef.position = { 8.0f, 0.2f };
			bodyDef.name = "debris";
			m_touchingBodyId = b3CreateBody( m_worldId, &bodyDef );

			b3Capsule capsule = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, 0.25f };

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 2.0f;

			b3CreateCapsuleShape( m_touchingBodyId, &shapeDef, &capsule );
		}

		// Create a separate floating body
		{
			b3BodyDef bodyDef = b3DefaultBodyDef();
			bodyDef.type = m_type;
			bodyDef.isEnabled = m_isEnabled;
			bodyDef.position = { -8.0f, 12.0f };
			bodyDef.gravityScale = 0.0f;
			bodyDef.name = "floater";
			m_floatingBodyId = b3CreateBody( m_worldId, &bodyDef );

			b3Sphere sphere = { { 0.0f, 0.5f, 0.0f }, 0.25f };

			b3ShapeDef shapeDef = b3DefaultShapeDef();
			shapeDef.density = 2.0f;

			b3CreateSphereShape( m_floatingBodyId, &shapeDef, &sphere );
		}
	}

	bool DrawControls() override
	{
		if ( ImGui::RadioButton( "Static", m_type == b3_staticBody ) )
		{
			m_type = b3_staticBody;
			b3Body_SetType( m_platformId, b3_staticBody );
			b3Body_SetType( m_secondAttachmentId, b3_staticBody );
			b3Body_SetType( m_secondPayloadId, b3_staticBody );
			b3Body_SetType( m_touchingBodyId, b3_staticBody );
			b3Body_SetType( m_floatingBodyId, b3_staticBody );
		}

		if ( ImGui::RadioButton( "Kinematic", m_type == b3_kinematicBody ) )
		{
			m_type = b3_kinematicBody;
			b3Body_SetType( m_platformId, b3_kinematicBody );
			b3Body_SetLinearVelocity( m_platformId, { -m_speed, 0.0f } );
			b3Body_SetAngularVelocity( m_platformId, b3Vec3_zero );

			b3Body_SetType( m_secondAttachmentId, b3_kinematicBody );
			b3Body_SetLinearVelocity( m_secondAttachmentId, b3Vec3_zero );
			b3Body_SetAngularVelocity( m_secondAttachmentId, b3Vec3_zero );

			b3Body_SetType( m_secondPayloadId, b3_kinematicBody );
			b3Body_SetType( m_touchingBodyId, b3_kinematicBody );
			b3Body_SetType( m_floatingBodyId, b3_kinematicBody );
		}

		if ( ImGui::RadioButton( "Dynamic", m_type == b3_dynamicBody ) )
		{
			m_type = b3_dynamicBody;
			b3Body_SetType( m_platformId, b3_dynamicBody );
			b3Body_SetType( m_secondAttachmentId, b3_dynamicBody );
			b3Body_SetType( m_secondPayloadId, b3_dynamicBody );
			b3Body_SetType( m_touchingBodyId, b3_dynamicBody );
			b3Body_SetType( m_floatingBodyId, b3_dynamicBody );
		}

		if ( ImGui::Checkbox( "Enable", &m_isEnabled ) )
		{
			if ( m_isEnabled )
			{
				b3Body_Enable( m_attachmentId );
				b3Body_Enable( m_secondPayloadId );
				b3Body_Enable( m_floatingBodyId );
			}
			else
			{
				b3Body_Disable( m_attachmentId );
				b3Body_Disable( m_secondPayloadId );
				b3Body_Disable( m_floatingBodyId );
			}
		}

		return true;
	}

	void Step() override
	{
		// Drive the kinematic body.
		if ( m_type == b3_kinematicBody )
		{
			b3Pos p = b3Body_GetPosition( m_platformId );
			b3Vec3 v = b3Body_GetLinearVelocity( m_platformId );

			if ( ( p.x < -14.0f && v.x < 0.0f ) || ( p.x > 6.0f && v.x > 0.0f ) )
			{
				v.x = -v.x;
				b3Body_SetLinearVelocity( m_platformId, v );
			}
		}

		Sample::Step();
	}

	static Sample* Create( SampleContext* context )
	{
		return new LoaderMjcf( context );
	}

	b3BodyId m_attachmentId;
	b3BodyId m_secondAttachmentId;
	b3BodyId m_platformId;
	b3BodyId m_secondPayloadId;
	b3BodyId m_touchingBodyId;
	b3BodyId m_floatingBodyId;
	b3BodyType m_type;
	float m_speed;
	bool m_isEnabled;
};

static int sampleMujocoMjcf = RegisterSample( "Mujoco", "Loader mjcf", LoaderMjcf::Create );
