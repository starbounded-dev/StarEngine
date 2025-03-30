using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using StarEngine;

namespace Sandbox
{
	public class Camera : Entity
	{
		void OnUpdate(float ts)
		{
			float speed = 1.0f;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.Up))
				velocity.y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.Down))
				velocity.y = -1.0f;

			if (Input.IsKeyDown(KeyCode.Left))
				velocity.x = -1.0f;
			else if (Input.IsKeyDown(KeyCode.Right))
				velocity.x = 1.0f;

			velocity *= speed;

			Vector3 translation = Translation;
			translation += velocity * ts;
			Translation = translation;
		}
	}
}
