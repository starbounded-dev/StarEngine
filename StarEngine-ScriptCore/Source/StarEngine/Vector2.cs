namespace StarEngine
{
	public struct Vector2
	{
		public float x, y;

		public static Vector2 Zero => new Vector2(0.0f);

		public Vector2(float scalar)
		{
			x = scalar;
			y = scalar;
		}

		public Vector2(float x, float y)
		{
			this.x = x;
			this.y = y;
		}

		public static Vector2 operator +(Vector2 left, Vector2 right)
		{
			return new Vector2(left.x + right.x, left.y + right.y);
		}

		public static Vector2 operator *(Vector2 left, float right)
		{
			return new Vector2(left.x * right, left.y * right);
		}
	}
}
