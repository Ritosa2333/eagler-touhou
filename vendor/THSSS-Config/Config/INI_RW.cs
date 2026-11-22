using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace Config
{
	public class INI_RW
	{
		[DllImport("kernel32")]
		private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

		[DllImport("kernel32")]
		private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

		public INI_RW(string INIPath)
		{
			this.inipath = INIPath;
		}

		public void IniWriteValue(string Section, string Key, string Value)
		{
			INI_RW.WritePrivateProfileString(Section, Key, Value, this.inipath);
		}

		public string IniReadValue(string Section, string Key)
		{
			StringBuilder stringBuilder = new StringBuilder(500);
			INI_RW.GetPrivateProfileString(Section, Key, "", stringBuilder, 500, this.inipath);
			return stringBuilder.ToString();
		}

		public bool ExistINIFile()
		{
			return File.Exists(this.inipath);
		}

		public string inipath;
	}
}
