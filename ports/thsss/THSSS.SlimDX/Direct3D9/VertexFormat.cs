using System;
namespace SlimDX.Direct3D9 {
	[Flags]
	public enum VertexFormat { None = 0, Position = 0x002, Normal = 0x010, Diffuse = 0x040, Texture1 = 0x100 }
}
