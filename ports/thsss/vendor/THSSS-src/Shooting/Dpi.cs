using System;
using System.Management;

namespace Shooting;

public class Dpi
{
	public static int PixelsPerXLogicalInch;

	public static int PixelsPerYLogicalInch;

	public static void GetDpi()
	{
		//IL_0005: Unknown result type (might be due to invalid IL or missing references)
		//IL_000b: Expected Obj, but got Unknown
		//IL_002d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0032: Unknown result type (might be due to invalid IL or missing references)
		ManagementClass val = new ManagementClass("Win32_DesktopMonitor");
		try
		{
			ManagementObjectCollection instances = val.GetInstances();
			try
			{
				PixelsPerXLogicalInch = 0;
				PixelsPerYLogicalInch = 0;
				ManagementObjectEnumerator enumerator = instances.GetEnumerator();
				try
				{
					while (enumerator.MoveNext())
					{
						ManagementObject val2 = (ManagementObject)enumerator.Current;
						PixelsPerXLogicalInch = int.Parse(((ManagementBaseObject)val2).Properties["PixelsPerXLogicalInch"].Value.ToString());
						PixelsPerYLogicalInch = int.Parse(((ManagementBaseObject)val2).Properties["PixelsPerYLogicalInch"].Value.ToString());
					}
				}
				finally
				{
					((IDisposable)enumerator)?.Dispose();
				}
			}
			finally
			{
				((IDisposable)instances)?.Dispose();
			}
		}
		finally
		{
			((IDisposable)val)?.Dispose();
		}
	}
}
