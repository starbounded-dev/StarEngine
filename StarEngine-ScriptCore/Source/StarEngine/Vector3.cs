namespace StarEngine
{
	public struct Vector3
	{
		public float x, y, z;

		public static Vector3 Zero => new Vector3(0.0f);

		public Vector3(float scalar)
		{
			x = scalar;
			y = scalar;
			z = scalar;
		}

		public Vector3(float x, float y, float z)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public Vector2 XY
		{
			get => new Vector2(x, y);
			set
			{
				x = value.x;
				y = value.y;
			}
		}

		public static Vector3 operator +(Vector3 left, Vector3 right)
		{
			return new Vector3(left.x + right.x, left.y + right.y, left.z + right.z);
		}

		public static Vector3 operator *(Vector3 left, float right)
		{
			return new Vector3(left.x * right, left.y * right, left.z * right);
		}
	}
}
