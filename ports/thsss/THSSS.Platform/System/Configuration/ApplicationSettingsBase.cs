namespace System.Configuration
{
	public class SettingsBase
	{
		public static SettingsBase Synchronized(SettingsBase settingsBase)
		{
			return settingsBase;
		}
	}

	public class ApplicationSettingsBase : SettingsBase
	{
	}
}
