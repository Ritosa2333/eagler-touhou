using System;

namespace SlimDX.Direct3D9
{
	public class PresentParameters
	{
		public int BackBufferWidth { get; set; }

		public int BackBufferHeight { get; set; }

		public Format BackBufferFormat { get; set; }

		public int BackBufferCount { get; set; }

		public IntPtr DeviceWindowHandle { get; set; }

		public MultisampleType Multisample { get; set; }

		public bool Windowed { get; set; }

		public PresentFlags PresentFlags { get; set; }

		public SwapEffect SwapEffect { get; set; }

		public PresentInterval PresentationInterval { get; set; }
	}
}
