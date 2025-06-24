namespace StarEngine
{
	public struct Vector4
	{
		public float X;
		public float Y;
		public float Z;
		public float W;

		/// <summary>
		/// Initializes a new instance of the Vector4 struct with all components set to the specified scalar value.
		/// </summary>
		/// <param name="scalar">The value to assign to all four components (X, Y, Z, W).</param>
		public Vector4(float scalar)
		{
			X = Y = Z = W = scalar;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="Vector4"/> struct with the specified component values.
		/// </summary>
		/// <param name="x">The value for the X component.</param>
		/// <param name="y">The value for the Y component.</param>
		/// <param name="z">The value for the Z component.</param>
		/// <param name="w">The value for the W component.</param>
		public Vector4(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		/// <summary>
		/// Initializes a new instance of the Vector4 struct using the components of a Vector3 for X, Y, and Z, and a separate float value for W.
		/// </summary>
		/// <param name="xyz">The Vector3 providing the X, Y, and Z components.</param>
		/// <param name="w">The value for the W component.</param>
		public Vector4(Vector3 xyz, float w)
		{
			X = xyz.X;
			Y = xyz.Y;
			Z = xyz.Z;
			W = w;
		}

		public Vector2 XY
		{
			get => new Vector2(X, Y);
			set
			{
				X = value.X;
				Y = value.Y;
			}
		}

		public Vector3 XYZ
		{
			get => new Vector3(X, Y, Z);
			set
			{
				X = value.X;
				Y = value.Y;
				Z = value.Z;
			}
		}

		public static Vector4 operator +(Vector4 left, Vector4 right)
		{
			return new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
		}

		public static Vector4 operator -(Vector4 left, Vector4 right)
		{
			return new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
		}

		public static Vector4 operator *(Vector4 left, Vector4 right)
		{
			return new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
		}

		public static Vector4 operator *(Vector4 left, float scalar)
		{
			return new Vector4(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);
		}

		public static Vector4 operator *(float scalar, Vector4 right)
		{
			return new Vector4(scalar * right.X, scalar * right.Y, scalar * right.Z, scalar * right.W);
		}

		public static Vector4 operator /(Vector4 left, Vector4 right)
		{
			return new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);
		}

		public static Vector4 operator /(Vector4 left, float scalar)
		{
			return new Vector4(left.X / scalar, left.Y / scalar, left.Z / scalar, left.W / scalar);
		}

		/// <summary>
		/// Returns the linear interpolation between two <see cref="Vector4"/> instances based on the interpolation factor <paramref name="t"/>.
		/// </summary>
		/// <param name="a">The starting <see cref="Vector4"/>.</param>
		/// <param name="b">The ending <see cref="Vector4"/>.</param>
		/// <param name="t">The interpolation factor, clamped between 0 and 1.</param>
		/// <returns>The interpolated <see cref="Vector4"/> between <paramref name="a"/> and <paramref name="b"/>.</returns>
		public static Vector4 Lerp(Vector4 a, Vector4 b, float t)
		{
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
			return (1.0f - t) * a + t * b;
		}

	}
}
