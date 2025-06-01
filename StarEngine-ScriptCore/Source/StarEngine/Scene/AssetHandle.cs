using System.Runtime.InteropServices;
using StarEngine;

namespace StarEngine
{
	[StructLayout(LayoutKind.Sequential)]
	public struct AssetHandle
	{
		public static readonly AssetHandle Invalid = new AssetHandle(0);

		internal ulong m_Handle;

		public AssetHandle(ulong handle) { m_Handle = handle; }

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

		public ulong GetHandle()
		{
			unsafe
			{
				return m_Handle;
			}
		}

		public void SetHandle(ulong handle)
		{
			unsafe
			{
				m_Handle = handle;
			}
		}

		public override string ToString() => m_Handle.ToString();
		public override int GetHashCode() => m_Handle.GetHashCode();
	}
}
