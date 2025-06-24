using System;

namespace StarEngine
{
	public static class Mathf
	{
		public const float Epsilon = 0.00001f;
		public const float PI = (float)Math.PI;
		public const float TwoPI = (float)(Math.PI * 2.0);

		public const float Deg2Rad = PI / 180.0f;
		public const float Rad2Deg = 180.0f / PI;

		/// <summary>
/// Returns the sine of the specified angle in radians.
/// </summary>
/// <param name="value">An angle, measured in radians.</param>
/// <returns>The sine of the input angle.</returns>
public static float Sin(float value) => (float)Math.Sin(value);
		/// <summary>
/// Returns the cosine of the specified angle in radians.
/// </summary>
/// <param name="value">An angle, measured in radians.</param>
/// <returns>The cosine of the input angle.</returns>
public static float Cos(float value) => (float)Math.Cos(value);
		/// <summary>
/// Returns the arc cosine of the specified value, in radians.
/// </summary>
/// <param name="value">A value between -1 and 1.</param>
/// <returns>The angle whose cosine is the specified value, in radians.</returns>
public static float Acos(float value) => (float)Math.Acos(value);

		/// <summary>
		/// Restricts a float value to lie within the inclusive range defined by <paramref name="min"/> and <paramref name="max"/>.
		/// </summary>
		/// <param name="value">The value to clamp.</param>
		/// <param name="min">The minimum allowable value.</param>
		/// <param name="max">The maximum allowable value.</param>
		/// <returns>The clamped value, which will not be less than <paramref name="min"/> or greater than <paramref name="max"/>.</returns>
		public static float Clamp(float value, float min, float max)
		{
			if (value < min)
				return min;
			return value > max ? max : value;
		}

		/// <summary>
/// Returns the arcsine of the specified value, in radians.
/// </summary>
/// <param name="x">A value between -1 and 1 representing the sine of an angle.</param>
/// <returns>The angle in radians whose sine is <paramref name="x"/>.</returns>
public static float Asin(float x) => (float)Math.Asin(x);
		/// <summary>
/// Returns the arctangent of the specified value, in radians.
/// </summary>
/// <param name="x">The value whose arctangent is to be calculated.</param>
/// <returns>The angle in radians whose tangent is <paramref name="x"/>.</returns>
public static float Atan(float x) => (float)Math.Atan(x);
		/// <summary>
/// Returns the angle in radians between the positive x-axis and the point (x, y).
/// </summary>
/// <param name="y">The y-coordinate.</param>
/// <param name="x">The x-coordinate.</param>
/// <returns>The angle in radians between the positive x-axis and the point (x, y).</returns>
public static float Atan2(float y, float x) => (float)Math.Atan2(y, x);

		/// <summary>
/// Returns the smaller of two float values.
/// </summary>
/// <param name="v0">The first value to compare.</param>
/// <param name="v1">The second value to compare.</param>
/// <returns>The lesser of <paramref name="v0"/> and <paramref name="v1"/>.</returns>
public static float Min(float v0, float v1) => v0 < v1 ? v0 : v1;
		/// <summary>
/// Returns the larger of two float values.
/// </summary>
/// <param name="v0">The first value to compare.</param>
/// <param name="v1">The second value to compare.</param>
/// <returns>The greater of <paramref name="v0"/> and <paramref name="v1"/>.</returns>
public static float Max(float v0, float v1) => v0 > v1 ? v0 : v1;

		/// <summary>
/// Returns the square root of the specified value.
/// </summary>
/// <param name="value">The value for which to compute the square root.</param>
/// <returns>The square root of <paramref name="value"/>.</returns>
public static float Sqrt(float value) => (float)Math.Sqrt(value);

		/// <summary>
/// Returns the absolute value of the specified floating-point number.
/// </summary>
/// <param name="value">The float value for which to compute the absolute value.</param>
/// <returns>The non-negative value of <paramref name="value"/>.</returns>
public static float Abs(float value) => Math.Abs(value);
		/// <summary>
/// Returns the absolute value of the specified integer.
/// </summary>
/// <param name="value">The integer whose absolute value is to be computed.</param>
/// <returns>The non-negative value of the input integer.</returns>
public static int Abs(int value) => Math.Abs(value);

		/// <summary>
		/// Returns a new Vector3 with each component set to the absolute value of the corresponding component in the input vector.
		/// </summary>
		/// <param name="value">The input vector.</param>
		/// <returns>A Vector3 with all components non-negative.</returns>
		public static Vector3 Abs(Vector3 value)
		{
			return new Vector3(Math.Abs(value.X), Math.Abs(value.Y), Math.Abs(value.Z));
		}

		/// <summary>
/// Performs linear interpolation between two float values based on the interpolation factor.
/// </summary>
/// <param name="p1">The start value.</param>
/// <param name="p2">The end value.</param>
/// <param name="t">The interpolation factor, typically between 0 and 1.</param>
/// <returns>The interpolated float value between <paramref name="p1"/> and <paramref name="p2"/>.</returns>
public static float Lerp(float p1, float p2, float t) => Interpolate.Linear(p1, p2, t);
		/// <summary>
/// Performs linear interpolation between two Vector3 values.
/// </summary>
/// <param name="p1">The starting Vector3 value.</param>
/// <param name="p2">The ending Vector3 value.</param>
/// <param name="t">The interpolation factor, typically between 0 and 1.</param>
/// <returns>The interpolated Vector3 value between <paramref name="p1"/> and <paramref name="p2"/>.</returns>
public static Vector3 Lerp(Vector3 p1, Vector3 p2, float t) => Interpolate.Linear(p1, p2, t);

		/// <summary>
/// Returns the largest integer less than or equal to the specified float value.
/// </summary>
/// <param name="value">The floating-point number to floor.</param>
/// <returns>The largest integer less than or equal to <paramref name="value"/>.</returns>
public static float Floor(float value) => (float)Math.Floor(value);

		/// <summary>
/// Computes the mathematical modulo of <paramref name="a"/> by <paramref name="b"/>, using floor division to ensure the result is always non-negative for positive divisors.
/// </summary>
/// <param name="a">The dividend.</param>
/// <param name="b">The divisor.</param>
/// <returns>The result of <c>a - b * floor(a / b)</c>, which may differ from the C# <c>%</c> operator for negative values.</returns>
		public static float Modulo(float a, float b) => a - b * (float)Math.Floor(a / b);

		/// <summary>
/// Returns the absolute difference between two float values.
/// </summary>
/// <param name="p1">The first value.</param>
/// <param name="p2">The second value.</param>
/// <returns>The distance between <paramref name="p1"/> and <paramref name="p2"/>.</returns>
public static float Distance(float p1, float p2) => Abs(p1 - p2);
	}

}
