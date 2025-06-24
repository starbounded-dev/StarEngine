#include "sepch.h"
#include "ContactListener2D.h"

#include "StarEngine/Scene/Entity.h"
#include "StarEngine/Scripting/ScriptEngine.h"

namespace StarEngine {

	/**
	 * @brief Handles the event when two 2D physics fixtures begin contact.
	 *
	 * If the engine is in play mode, retrieves the entities involved in the collision and invokes their `OnCollisionBegin` script callbacks if they have both a `ScriptComponent` with a valid instance and a `RigidBody2DComponent`.
	 */
	void ContactListener2D::BeginContact(b2Contact* contact)
	{
		if (m_IsPlaying)
		{
			Entity& a = *(Entity*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			Entity& b = *(Entity*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			if (a.HasComponent<ScriptComponent>() && a.HasComponent<RigidBody2DComponent>())
			{
				auto& scriptComponent = a.GetComponent<ScriptComponent>();
				if (scriptComponent.Instance.IsValid()) // Ensure the instance is valid
				{
					scriptComponent.Instance.Invoke("OnCollisionBegin");
				}
			}

			if (b.HasComponent<ScriptComponent>() && b.HasComponent<RigidBody2DComponent>())
			{
				auto& scriptComponent = b.GetComponent<ScriptComponent>();
				if (scriptComponent.Instance.IsValid()) // Ensure the shared_ptr is not null
				{
					scriptComponent.Instance.Invoke("OnCollisionBegin");
				}
			}

		}
	}

	/**
	 * @brief Handles the end of contact between two 2D physics fixtures.
	 *
	 * Invokes the `OnCollisionEnd` callback on script instances attached to entities involved in the collision, provided the engine is in play mode and the entities have both `ScriptComponent` and `RigidBody2DComponent` with a valid script instance.
	 */
	void ContactListener2D::EndContact(b2Contact* contact)
	{
		if (m_IsPlaying)
		{
			Entity& a = *(Entity*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			Entity& b = *(Entity*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			if (a.HasComponent<ScriptComponent>() && a.HasComponent<RigidBody2DComponent>())
			{
				auto& scriptComponent = a.GetComponent<ScriptComponent>();
				if (scriptComponent.Instance.IsValid()) // Ensure the instance is valid
				{
					scriptComponent.Instance.Invoke("OnCollisionEnd");
				}
			}

			if (b.HasComponent<ScriptComponent>() && b.HasComponent<RigidBody2DComponent>())
			{
				auto& scriptComponent = b.GetComponent<ScriptComponent>();
				if (scriptComponent.Instance.IsValid()) // Ensure the instance is valid
				{
					scriptComponent.Instance.Invoke("OnCollisionEnd");

				}
			}
		}
	}

}
