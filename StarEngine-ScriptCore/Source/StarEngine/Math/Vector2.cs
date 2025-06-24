using System;

namespace StarEngine
{
	public struct Vector2
	{
		public static Vector2 Zero = new Vector2(0, 0);
		public static Vector2 One = new Vector2(1, 1);

		public static Vector2 Up = new Vector2(0, 1);
		public static Vector2 Down = new Vector2(0, -1);
		public static Vector2 Left = new Vector2(-1, 0);
		public static Vector2 Right = new Vector2(1, 0);

		public float X;
		public float Y;

		/// <summary>
		/// Initializes a new Vector2 with both X and Y components set to the specified scalar value.
		/// </summary>
		/// <param name="scalar">The value to assign to both X and Y components.</param>
		public Vector2(float scalar)
		{
			X = Y = scalar;
		}

		/// <summary>
		/// Initializes a new instance of the Vector2 struct with the specified X and Y components.
		/// </summary>
		/// <param name="x">The X component of the vector.</param>
		/// <param name="y">The Y component of the vector.</param>
		public Vector2(float x, float y)
		{
			X = x;
			Y = y;
		}

		/// <summary>
		/// Initializes a new Vector2 using the X and Y components of the specified Vector3.
		/// </summary>
		/// <param name="vector">The Vector3 from which to take the X and Y components.</param>
		public Vector2(Vector3 vector)
		{
			X = vector.X;
			Y = vector.Y;
		}

		/// <summary>
		/// Calculates the Euclidean distance between this vector and the specified <see cref="Vector3"/>, considering only the X and Y components.
		/// </summary>
		/// <param name="other">The <see cref="Vector3"/> to measure the distance to.</param>
		/// <returns>The distance between this vector and <paramref name="other"/> in the XY plane.</returns>
		public float Distance(Vector3 other)
		{
			return (float)Math.Sqrt(Math.Pow(other.X - X, 2) +
									Math.Pow(other.Y - Y, 2));
		}

		/// <summary>
		/// Calculates the Euclidean distance between two <see cref="Vector3"/> points, considering only their X and Y components.
		/// </summary>
		/// <param name="p1">The first <see cref="Vector3"/> point.</param>
		/// <param name="p2">The second <see cref="Vector3"/> point.</param>
		/// <returns>The distance between <paramref name="p1"/> and <paramref name="p2"/> in the XY plane.</returns>
		public static float Distance(Vector3 p1, Vector3 p2)
		{
			return (float)Math.Sqrt(Math.Pow(p2.X - p1.X, 2) +
									Math.Pow(p2.Y - p1.Y, 2));
		}

		/// <summary>
		/// Clamps the X and Y components of this vector between the corresponding components of the specified minimum and maximum vectors.
		/// </summary>
		/// <param name="min">The vector specifying the minimum allowed values for each component.</param>
		/// <param name="max">The vector specifying the maximum allowed values for each component.</param>
		public void Clamp(Vector2 min, Vector2 max)
		{
			X = Mathf.Clamp(X, min.X, max.X);
			Y = Mathf.Clamp(Y, min.Y, max.Y);
		}

		public static Vector2 operator +(Vector2 left, Vector2 right)
		{
			return new Vector2(left.X + right.X, left.Y + right.Y);
		}

		public static Vector2 operator -(Vector2 left, Vector2 right)
		{
			return new Vector2(left.X - right.X, left.Y - right.Y);
		}

		public static Vector2 operator -(Vector2 vector)
		{
			return new Vector2(-vector.X, -vector.Y);
		}

		public static Vector2 operator *(Vector2 left, float scalar)
		{
			return new Vector2(left.X * scalar, left.Y * scalar);
		}

		public static Vector2 operator *(float scalar, Vector2 right)
		{
			return new Vector2(scalar * right.X, scalar * right.Y);
		}

		/// <summary>
/// Determines whether the specified object is a <see cref="Vector2"/> and has the same component values as this instance.
/// </summary>
/// <param name="obj">The object to compare with this vector.</param>
/// <returns><c>true</c> if the specified object is a <see cref="Vector2"/> with equal <c>X</c> and <c>Y</c> components; otherwise, <c>false</c>.</returns>
public override bool Equals(object obj) => obj is Vector2 other && this.Equals(other);

		/// <summary>
/// Determines whether this vector is equal to another vector by comparing their components.
/// </summary>
/// <param name="right">The vector to compare with this instance.</param>
/// <returns>True if both vectors have the same X and Y components; otherwise, false.</returns>
public bool Equals(Vector2 right) => X == right.X && Y == right.Y;

		/// <summary>
/// Returns a hash code based on the vector's X and Y components.
/// </summary>
/// <returns>A hash code for the current vector.</returns>
public override int GetHashCode() => (X, Y).GetHashCode();

		public static bool operator ==(Vector2 left, Vector2 right) => left.Equals(right);
		public static bool operator !=(Vector2 left, Vector2 right) => !(left == right);

		/// <summary>
		/// Linearly interpolates between two vectors by a specified interpolation factor clamped between 0 and 1.
		/// </summary>
		/// <param name="p1">The starting vector.</param>
		/// <param name="p2">The ending vector.</param>
		/// <param name="maxDistanceDelta">The interpolation factor; values less than 0 return <paramref name="p1"/>, values greater than 1 return <paramref name="p2"/>.</param>
		/// <returns>The interpolated vector between <paramref name="p1"/> and <paramref name="p2"/>.</returns>
		public static Vector2 Lerp(Vector2 p1, Vector2 p2, float maxDistanceDelta)
		{
			if (maxDistanceDelta < 0.0f)
				return p1;
			if (maxDistanceDelta > 1.0f)
				return p2;

			return p1 + ((p2 - p1) * maxDistanceDelta);
		}

		/// <summary>
		/// Returns the product of the X and Y components of the specified vector.
		/// </summary>
		/// <param name="vec">The vector whose X and Y components will be multiplied.</param>
		/// <returns>The result of multiplying vec.X by vec.Y.</returns>
		public float Dot(Vector2 vec)
		{
			return (float)(vec.X * vec.Y);
		}

		/// <summary>
		/// Returns the squared length (magnitude) of the vector.
		/// </summary>
		/// <returns>The sum of the squares of the X and Y components.</returns>
		public float LengthSquared()
		{
			return X * X + Y * Y;
		}

		/// <summary>
		/// Returns the Euclidean length (magnitude) of the vector.
		/// </summary>
		/// <returns>The length of the vector.</returns>
		public float Length()
		{
			return (float)Math.Sqrt(LengthSquared());
		}

		/// <summary>
		/// Returns a new vector with the same direction as this vector but with a length of 1, or the original vector if its length is zero.
		/// </summary>
		/// <returns>A normalized vector, or the original vector if its length is zero.</returns>
		public Vector2 Normalized()
		{
			float length = Length();
			float x = X;
			float y = Y;

			if (length > 0.0f)
			{
				x /= length;
				y /= length;
			}

			return new Vector2(x, y);
		}

		/// <summary>
		/// Normalizes the vector in place to have a length of 1, if its current length is greater than zero.
		/// </summary>
		public void Normalize()
		{
			float length = Length();

			if (length > 0.0f)
			{
				X = X / length;
				Y = Y / length;
			}
		}

		/// <summary>
		/// Returns a string representation of the vector in the format "Vector2[X, Y]".
		/// </summary>
		/// <returns>A string describing the vector's components.</returns>
		public override string ToString()
		{
			return "Vector2[" + X + ", " + Y + "]";
		}
	}
}
