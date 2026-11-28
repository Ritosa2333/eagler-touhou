using System;
namespace SlimDX.Direct3D9 {
	[Flags]
	public enum LockFlags { None = 0, Discard = 0x2000, NoOverwrite = 0x1000, ReadOnly = 16 }
}
