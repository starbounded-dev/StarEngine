using System;

namespace StarEngine
{
	public struct Vector3
	{
		public static Vector3 Zero = new Vector3(0, 0, 0);
		public static Vector3 One = new Vector3(1, 1, 1);

		public static Vector3 Forward = new Vector3(0, 0, -1);
		public static Vector3 Right = new Vector3(1, 0, 0);
		public static Vector3 Up = new Vector3(0, 1, 0);
		public static Vector3 Down = new Vector3(0, -1, 0);
		public static Vector3 Left = new Vector3(-1, 0, 0);
		public static Vector3 Back = new Vector3(0, 0, 1);

		public float X;
		public float Y;
		public float Z;

		/// <summary>
		/// Initializes a new Vector3 with all components set to the specified scalar value.
		/// </summary>
		/// <param name="scalar">The value to assign to the X, Y, and Z components.</param>
		public Vector3(float scalar)
		{
			X = Y = Z = scalar;
		}

		/// <summary>
		/// Initializes a new instance of the Vector3 struct with the specified X, Y, and Z components.
		/// </summary>
		/// <param name="x">The X component of the vector.</param>
		/// <param name="y">The Y component of the vector.</param>
		/// <param name="z">The Z component of the vector.</param>
		public Vector3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		/// <summary>
		/// Initializes a new instance of the Vector3 struct with the specified X component and YZ components from a Vector2.
		/// </summary>
		/// <param name="x">The X component value.</param>
		/// <param name="yz">A Vector2 whose X and Y values are used for the Y and Z components, respectively.</param>
		public Vector3(float x, Vector2 yz)
		{
			X = x;
			Y = yz.X;
			Z = yz.Y;
		}

		/// <summary>
		/// Initializes a new instance of the Vector3 struct using the X and Y components from a Vector2 and a specified Z component.
		/// </summary>
		/// <param name="xy">A Vector2 providing the X and Y components.</param>
		/// <param name="z">The value for the Z component.</param>
		public Vector3(Vector2 xy, float z)
		{
			X = xy.X;
			Y = xy.Y;
			Z = z;
		}

		/// <summary>
		/// Initializes a new Vector3 using the X and Y components from a Vector2, setting Z to 0.
		/// </summary>
		/// <param name="vector">The Vector2 providing the X and Y values.</param>
		public Vector3(Vector2 vector)
		{
			X = vector.X;
			Y = vector.Y;
			Z = 0.0f;
		}

		/// <summary>
		/// Initializes a new Vector3 using the X, Y, and Z components of the given Vector4.
		/// </summary>
		/// <param name="vector">The Vector4 from which to copy the X, Y, and Z values.</param>
		public Vector3(Vector4 vector)
		{
			X = vector.X;
			Y = vector.Y;
			Z = vector.Z;
		}

		/// <summary>
		/// Restricts each component of the vector to be within the corresponding minimum and maximum values.
		/// </summary>
		/// <param name="min">The minimum values for each component.</param>
		/// <param name="max">The maximum values for each component.</param>
		public void Clamp(Vector3 min, Vector3 max)
		{
			X = Mathf.Clamp(X, min.X, max.X);
			Y = Mathf.Clamp(Y, min.Y, max.Y);
			Z = Mathf.Clamp(Z, min.Z, max.Z);
		}

		/// <summary>
		/// Calculates the Euclidean length (magnitude) of the vector.
		/// </summary>
		/// <returns>The length of the vector.</returns>
		public float Length()
		{
			return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
		}

		/// <summary>
		/// Returns a normalized copy of this vector with a length of 1, or the original vector if its length is zero.
		/// </summary>
		/// <returns>A unit vector in the same direction as this vector, or the original vector if its length is zero.</returns>
		public Vector3 Normalized()
		{
			float length = Length();
			float x = X;
			float y = Y;
			float z = Z;

			if (length > 0.0f)
			{
				x /= length;
				y /= length;
				z /= length;
			}

			return new Vector3(x, y, z);
		}

		/// <summary>
		/// Normalizes the vector in place to have a length of 1 if its current length is greater than zero.
		/// </summary>
		public void Normalize()
		{
			float length = Length();

			if (length > 0.0f)
			{
				X = X / length;
				Y = Y / length;
				Z = Z / length;
			}
		}

		/// <summary>
		/// Calculates the Euclidean distance between this vector and another vector.
		/// </summary>
		/// <param name="other">The vector to which the distance is measured.</param>
		/// <returns>The Euclidean distance between the two vectors.</returns>
		public float Distance(Vector3 other)
		{
			return (float)Math.Sqrt(Math.Pow(other.X - X, 2) +
											Math.Pow(other.Y - Y, 2) +
											Math.Pow(other.Z - Z, 2));
		}

		/// <summary>
		/// Calculates the Euclidean distance between two 3D vectors.
		/// </summary>
		/// <param name="p1">The first vector.</param>
		/// <param name="p2">The second vector.</param>
		/// <returns>The distance between <paramref name="p1"/> and <paramref name="p2"/>.</returns>
		public static float Distance(Vector3 p1, Vector3 p2)
		{
			return (float)Math.Sqrt(Math.Pow(p2.X - p1.X, 2) +
										Math.Pow(p2.Y - p1.Y, 2) +
										Math.Pow(p2.Z - p1.Z, 2));
		}

		/// <summary>
		/// Linearly interpolates between two vectors by a specified factor clamped between 0 and 1.
		/// </summary>
		/// <param name="p1">The starting vector.</param>
		/// <param name="p2">The ending vector.</param>
		/// <param name="maxDistanceDelta">Interpolation factor; values less than 0 return <paramref name="p1"/>, values greater than 1 return <paramref name="p2"/>.</param>
		/// <returns>The interpolated vector between <paramref name="p1"/> and <paramref name="p2"/>.</returns>
		public static Vector3 Lerp(Vector3 p1, Vector3 p2, float maxDistanceDelta)
		{
			if (maxDistanceDelta < 0.0f)
				return p1;
			if (maxDistanceDelta > 1.0f)
				return p2;

			return p1 + ((p2 - p1) * maxDistanceDelta);
		}

		public static Vector3 operator *(Vector3 left, float scalar)
		{
			return new Vector3(left.X * scalar, left.Y * scalar, left.Z * scalar);
		}

		public static Vector3 operator *(float scalar, Vector3 right)
		{
			return new Vector3(scalar * right.X, scalar * right.Y, scalar * right.Z);
		}

		public static Vector3 operator *(Vector3 left, Vector3 right)
		{
			return new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
		}

		public static Vector3 operator +(Vector3 left, Vector3 right)
		{
			return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
		}

		public static Vector3 operator +(Vector3 left, float right)
		{
			return new Vector3(left.X + right, left.Y + right, left.Z + right);
		}

		public static Vector3 operator -(Vector3 left, Vector3 right)
		{
			return new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
		}

		public static Vector3 operator /(Vector3 left, Vector3 right)
		{
			return new Vector3(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
		}

		public static Vector3 operator /(Vector3 left, float scalar)
		{
			return new Vector3(left.X / scalar, left.Y / scalar, left.Z / scalar);
		}

		public static Vector3 operator -(Vector3 vector)
		{
			return new Vector3(-vector.X, -vector.Y, -vector.Z);
		}

		/// <summary>
/// Determines whether the specified object is a Vector3 and has the same component values as this instance.
/// </summary>
/// <param name="obj">The object to compare with this Vector3.</param>
/// <returns>True if the object is a Vector3 with equal X, Y, and Z components; otherwise, false.</returns>
public override bool Equals(object obj) => obj is Vector3 other && this.Equals(other);

		/// <summary>
/// Determines whether this vector is equal to another vector by comparing each component.
/// </summary>
/// <param name="right">The vector to compare with this instance.</param>
/// <returns>True if all corresponding components are equal; otherwise, false.</returns>
public bool Equals(Vector3 right) => X == right.X && Y == right.Y && Z == right.Z;

		/// <summary>
/// Returns a hash code for the vector based on its X, Y, and Z components.
/// </summary>
/// <returns>An integer hash code representing the vector.</returns>
public override int GetHashCode() => (X, Y, Z).GetHashCode();

		public static bool operator ==(Vector3 left, Vector3 right) => left.Equals(right);

		public static bool operator !=(Vector3 left, Vector3 right) => !(left == right);

		/// <summary>
		/// Returns a new vector whose components are the cosine of the corresponding components of the input vector.
		/// </summary>
		/// <param name="vector">The input vector whose components will be used as angles in radians.</param>
		/// <returns>A vector with each component set to the cosine of the corresponding component of the input vector.</returns>
		public static Vector3 Cos(Vector3 vector)
		{
			return new Vector3((float)Math.Cos(vector.X), (float)Math.Cos(vector.Y), (float)Math.Cos(vector.Z));
		}

		/// <summary>
		/// Returns a new vector whose components are the sine of the corresponding components of the input vector.
		/// </summary>
		/// <param name="vector">The input vector whose components will be used as arguments to the sine function.</param>
		/// <returns>A vector with each component set to the sine of the corresponding component of the input vector.</returns>
		public static Vector3 Sin(Vector3 vector)
		{
			return new Vector3((float)Math.Sin(vector.X), (float)Math.Sin(vector.Y), (float)Math.Sin(vector.Z));
		}

		/// <summary>
		/// Returns a string representation of the vector in the format "Vector3[X, Y, Z]".
		/// </summary>
		/// <returns>A string describing the vector's components.</returns>
		public override string ToString()
		{
			return "Vector3[" + X + ", " + Y + ", " + Z + "]";
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
		public Vector2 XZ
		{
			get => new Vector2(X, Z);
			set
			{
				X = value.X;
				Z = value.Y;
			}
		}
		public Vector2 YZ
		{
			get => new Vector2(Y, Z);
			set
			{
				Y = value.X;
				Z = value.Y;
			}
		}

	}
}
