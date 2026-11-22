using System.Runtime.InteropServices;

namespace SlimDX
{
	[StructLayout(LayoutKind.Sequential, Pack = 4)]
	public struct Vector4
	{
		public float X;

		public float Y;

		public float Z;

		public float W;

		public Vector4(float x, float y, float z, float w)
		{
			this.X = x;
			this.Y = y;
			this.Z = z;
			this.W = w;
		}
	}
}
