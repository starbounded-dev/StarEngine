using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using StarEngine;

namespace Sandbox
{
	public class Player : Entity
	{
		private TransformComponent m_Transform;
		private RigidBody2DComponent m_RigidBody;

		void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate - {ID}");

			m_Transform = GetComponent<TransformComponent>();
			m_RigidBody = GetComponent<RigidBody2DComponent>();
		}

		void OnUpdate(float ts)
		{
			// Console.WriteLine($"Player.OnUpdate - {ID}");

			float speed = 0.01f;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.W))
				velocity.y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.y = -1.0f;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.x = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.x = 1.0f;

			velocity *= speed;

			m_RigidBody.ApplyLinearImpulse(velocity.XY, Vector2.Zero, true);
		}
	}
}
