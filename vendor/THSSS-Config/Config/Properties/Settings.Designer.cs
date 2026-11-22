using System;
using System.CodeDom.Compiler;
using System.Configuration;
using System.Runtime.CompilerServices;

namespace Config.Properties
{
	[GeneratedCode("Microsoft.VisualStudio.Editors.SettingsDesigner.SettingsSingleFileGenerator", "10.0.0.0")]
	[CompilerGenerated]
	internal sealed partial class Settings : ApplicationSettingsBase
	{
		public static Settings Default
		{
			get
			{
				return Settings.defaultInstance;
			}
		}

		private static Settings defaultInstance = (Settings)SettingsBase.Synchronized(new Settings());
	}
}
