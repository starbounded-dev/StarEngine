using System.Runtime.InteropServices;
using StarEngine;

namespace StarEngine
{
	[StructLayout(LayoutKind.Sequential)]
	public struct AssetHandle
	{
		public static readonly AssetHandle Invalid = new AssetHandle(0);

		internal ulong m_Handle;

		/// <summary>
/// Initializes a new instance of the <see cref="AssetHandle"/> struct with the specified handle value.
/// </summary>
/// <param name="handle">The underlying handle value to associate with this asset handle.</param>
public AssetHandle(ulong handle) { m_Handle = handle; }

		/// <summary>
		/// Determines whether the asset handle refers to a valid asset.
		/// </summary>
		/// <returns>True if the handle is valid; otherwise, false.</returns>
		public bool IsValid()
		{
			unsafe
			{
				return InternalCalls.AssetHandle_IsValid(this);
			}
		}

		public static implicit operator bool(AssetHandle assetHandle)
		{
			unsafe
			{
				return InternalCalls.AssetHandle_IsValid(assetHandle);
			}
		}

		/// <summary>
		/// Returns the underlying handle value represented by this AssetHandle.
		/// </summary>
		/// <returns>The current handle value as an unsigned long integer.</returns>
		public ulong GetHandle()
		{
			unsafe
			{
				return m_Handle;
			}
		}

		/// <summary>
		/// Sets the internal handle value to the specified value.
		/// </summary>
		/// <param name="handle">The new handle value to assign.</param>
		public void SetHandle(ulong handle)
		{
			unsafe
			{
				m_Handle = handle;
			}
		}

		/// <summary>
/// Returns the string representation of the underlying handle value.
/// </summary>
/// <returns>A string that represents the current handle.</returns>
public override string ToString() => m_Handle.ToString();
		/// <summary>
/// Returns the hash code for the asset handle based on its underlying value.
/// </summary>
/// <returns>The hash code of the internal handle value.</returns>
public override int GetHashCode() => m_Handle.GetHashCode();
	}
}
