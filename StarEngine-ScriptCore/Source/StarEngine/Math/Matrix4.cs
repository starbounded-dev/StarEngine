using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace StarEngine
{
	[StructLayout(LayoutKind.Explicit)]
	public struct Matrix4
	{
		[FieldOffset(0)] public float D00;
		[FieldOffset(4)] public float D10;
		[FieldOffset(8)] public float D20;
		[FieldOffset(12)] public float D30;
		[FieldOffset(16)] public float D01;
		[FieldOffset(20)] public float D11;
		[FieldOffset(24)] public float D21;
		[FieldOffset(28)] public float D31;
		[FieldOffset(32)] public float D02;
		[FieldOffset(36)] public float D12;
		[FieldOffset(40)] public float D22;
		[FieldOffset(44)] public float D32;
		[FieldOffset(48)] public float D03;
		[FieldOffset(52)] public float D13;
		[FieldOffset(56)] public float D23;
		[FieldOffset(60)] public float D33;

		/// <summary>
		/// Initializes a 4x4 matrix as a scaled identity matrix with the specified value on the diagonal.
		/// </summary>
		/// <param name="value">The value to set on the diagonal elements of the matrix.</param>
		public Matrix4(float value)
		{
			D00 = value; D10 = 0.0f; D20 = 0.0f; D30 = 0.0f;
			D01 = 0.0f; D11 = value; D21 = 0.0f; D31 = 0.0f;
			D02 = 0.0f; D12 = 0.0f; D22 = value; D32 = 0.0f;
			D03 = 0.0f; D13 = 0.0f; D23 = 0.0f; D33 = value;
		}

		public Vector3 Translation
		{
			get { return new Vector3(D03, D13, D23); }
			set { D03 = value.X; D13 = value.Y; D23 = value.Z; }
		}

		/// <summary>
		/// Creates a translation matrix with the specified translation vector.
		/// </summary>
		/// <param name="translation">The translation vector to apply.</param>
		/// <returns>A 4x4 matrix representing the translation transformation.</returns>
		public static Matrix4 Translate(Vector3 translation)
		{
			Matrix4 result = new Matrix4(1.0f);
			result.D03 = translation.X;
			result.D13 = translation.Y;
			result.D23 = translation.Z;
			return result;
		}

		/// <summary>
		/// Creates a scaling matrix with the specified scale factors applied to the X, Y, and Z axes.
		/// </summary>
		/// <param name="scale">A vector specifying the scale factors for each axis.</param>
		/// <returns>A 4x4 matrix representing the scaling transformation.</returns>
		public static Matrix4 Scale(Vector3 scale)
		{
			Matrix4 result = new Matrix4(1.0f);
			result.D00 = scale.X;
			result.D11 = scale.Y;
			result.D22 = scale.Z;
			return result;
		}

		/// <summary>
		/// Creates a uniform scaling matrix with the specified scale factor applied to the X, Y, and Z axes.
		/// </summary>
		/// <param name="scale">The uniform scale factor to apply.</param>
		/// <returns>A 4x4 matrix representing uniform scaling.</returns>
		public static Matrix4 Scale(float scale)
		{
			Matrix4 result = new Matrix4(1.0f);
			result.D00 = scale;
			result.D11 = scale;
			result.D22 = scale;
			return result;
		}

	}
}
