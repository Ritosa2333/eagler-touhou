namespace SlimDX.Direct3D9
{
	public class Direct3D : ComObject
	{
		public AdapterCollection Adapters { get; private set; }

		public Direct3D()
		{
			var displayMode = default(DisplayMode);
			displayMode.Width = 640;
			displayMode.Height = 480;
			displayMode.Format = Format.X8R8G8B8;
			this.Adapters = new AdapterCollection
			{
				DefaultAdapter = new AdapterInformation
				{
					Adapter = 0,
					CurrentDisplayMode = displayMode
				}
			};
		}

		public Capabilities GetDeviceCaps(int adapter, DeviceType deviceType)
		{
			return new Capabilities
			{
				DeviceCaps = DeviceCaps.HWTransformAndLight
			};
		}
	}
}
