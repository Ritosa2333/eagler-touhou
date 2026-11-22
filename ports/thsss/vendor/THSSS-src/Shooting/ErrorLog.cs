using System;
using System.IO;

namespace Shooting;

internal class ErrorLog
{
	public static void SaveErrorLog(string Message, string Description)
	{
		string text = null;
		text += DateTime.Now.ToString("yyyy'/'MM'/'dd HH:mm");
		text += "\r\n";
		text += Message;
		text += "\r\n";
		text += Description;
		text += "\r\n";
		string path = "Log.txt";
		StreamWriter streamWriter = (File.Exists(path) ? new StreamWriter(path, append: true) : File.CreateText(path));
		streamWriter.Write(text);
		streamWriter.Close();
	}
}
