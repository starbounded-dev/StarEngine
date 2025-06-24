namespace StarEngine
{
	public static class Interpolate
	{

		/// <summary>
		/// Performs linear interpolation between two float values, clamping the interpolation parameter to the [0, 1] range.
		/// </summary>
		/// <param name="p1">The start value.</param>
		/// <param name="p2">The end value.</param>
		/// <param name="t">The interpolation parameter, typically between 0 and 1.</param>
		/// <returns>The interpolated float value. Returns <paramref name="p1"/> if <paramref name="t"/> is less than 0, <paramref name="p2"/> if greater than 1, or the linear blend otherwise.</returns>
		public static float Linear(float p1, float p2, float t)
		{
			if (t < 0.0f)
				return p1;
			else if (t > 1.0f)
				return p2;

			return p1 + ((p2 - p1) * t);
		}

		/// <summary>
		/// Performs linear interpolation between two <see cref="Vector3"/> values based on the parameter <paramref name="t"/>.
		/// </summary>
		/// <param name="p1">The starting <see cref="Vector3"/> value.</param>
		/// <param name="p2">The ending <see cref="Vector3"/> value.</param>
		/// <param name="t">The interpolation factor, where 0 returns <paramref name="p1"/>, 1 returns <paramref name="p2"/>, and values in between return the interpolated result. Values less than 0 return <paramref name="p1"/>, and values greater than 1 return <paramref name="p2"/>.</param>
		/// <returns>The interpolated <see cref="Vector3"/> value.</returns>
		public static Vector3 Linear(Vector3 p1, Vector3 p2, float t)
		{
			if (t < 0.0f)
				return p1;
			else if (t > 1.0f)
				return p2;

			return p1 + ((p2 - p1) * t);
		}

		/// <summary>
		/// Performs an ease-in interpolation between two Vector3 values using a quadratic timing function.
		/// </summary>
		/// <param name="p1">The starting Vector3 value.</param>
		/// <param name="p2">The ending Vector3 value.</param>
		/// <param name="t">The interpolation parameter, typically in the range [0, 1].</param>
		/// <returns>The interpolated Vector3 value with an ease-in effect applied to the transition.</returns>
		public static Vector3 EaseIn(Vector3 p1, Vector3 p2, float t)
		{
			return Linear(p1, p2, EaseIn_Time(t));
		}

		/// <summary>
		/// Performs an ease-out interpolation between two Vector3 values.
		/// </summary>
		/// <param name="p1">The starting Vector3 value.</param>
		/// <param name="p2">The ending Vector3 value.</param>
		/// <param name="t">The interpolation parameter, typically between 0 and 1.</param>
		/// <returns>The interpolated Vector3 value using an ease-out timing function.</returns>
		public static Vector3 EaseOut(Vector3 p1, Vector3 p2, float t)
		{
			return Linear(p1, p2, EaseOut_Time(t));
		}

		/// <summary>
		/// Returns an eased-in-out interpolation between two float values using a quadratic ease-in-out timing function.
		/// </summary>
		/// <param name="a">The starting float value.</param>
		/// <param name="b">The ending float value.</param>
		/// <param name="t">The interpolation parameter, typically in the range [0, 1].</param>
		/// <returns>The interpolated float value with ease-in-out applied to the interpolation parameter.</returns>
		public static float EaseInOut(float a, float b, float t)
		{
			return Linear(a, b, EaseInOut_Time(t));
		}

		/// <summary>
		/// Returns an eased-in-out interpolation between two Vector3 values using a quadratic ease-in-out timing function.
		/// </summary>
		/// <param name="p1">The starting Vector3 value.</param>
		/// <param name="p2">The ending Vector3 value.</param>
		/// <param name="t">The interpolation parameter, typically in the range [0, 1].</param>
		/// <returns>The interpolated Vector3 value with ease-in-out applied to the interpolation parameter.</returns>
		public static Vector3 EaseInOut(Vector3 p1, Vector3 p2, float t)
		{
			return Linear(p1, p2, EaseInOut_Time(t));
		}

		/// <summary>
		/// Applies an ease-in timing function by returning the square of the input value.
		/// </summary>
		/// <param name="t">The interpolation parameter, typically in the range [0, 1].</param>
		/// <returns>The squared value of <paramref name="t"/>, representing an ease-in curve.</returns>
		private static float EaseIn_Time(float t)
		{
			return Square(t);
		}

		/// <summary>
		/// Calculates the ease-out timing value for the given parameter by applying an inverted quadratic curve.
		/// </summary>
		/// <param name="t">The interpolation parameter, typically in the range [0, 1].</param>
		/// <returns>The eased-out value corresponding to the input parameter.</returns>
		private static float EaseOut_Time(float t)
		{
			return Invert(Square(Invert(t)));
		}

		/// <summary>
		/// Computes an ease-in-out timing value by blending the ease-in and ease-out curves based on the parameter t.
		/// </summary>
		/// <param name="t">The interpolation parameter, typically in the range [0, 1].</param>
		/// <returns>The eased timing value for use in interpolation.</returns>
		private static float EaseInOut_Time(float t)
		{
			return Linear(EaseIn_Time(t), EaseOut_Time(t), t);
		}

		/// <summary>
		/// Returns the inverse of the specified value, calculated as 1.0 minus <paramref name="x"/>.
		/// </summary>
		/// <param name="x">The value to invert.</param>
		/// <returns>The result of 1.0f - x.</returns>
		internal static float Invert(float x)
		{
			return 1.0f - x;
		}

		/// <summary>
		/// Returns the square of the specified value.
		/// </summary>
		/// <param name="x">The value to be squared.</param>
		/// <returns>The result of x multiplied by itself.</returns>
		internal static float Square(float x)
		{
			return x * x;
		}

	}
}
