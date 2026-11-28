using System;
namespace SlimDX.Direct3D9 {
	[Flags]
	public enum Usage { None = 0, RenderTarget = 1, DepthStencil = 2, WriteOnly = 8 }
}
