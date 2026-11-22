using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace Shooting;

public class INI_RW
{
	public string inipath;

	[DllImport("kernel32")]
	private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

	[DllImport("kernel32")]
	private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

	public INI_RW(string INIPath)
	{
		inipath = INIPath;
	}

	public void IniWriteValue(string Section, string Key, string Value)
	{
		WritePrivateProfileString(Section, Key, Value, inipath);
	}

	public string IniReadValue(string Section, string Key)
	{
		StringBuilder stringBuilder = new StringBuilder(500);
		GetPrivateProfileString(Section, Key, "", stringBuilder, 500, inipath);
		return stringBuilder.ToString();
	}

	public bool ExistINIFile()
	{
		return File.Exists(inipath);
	}
}
