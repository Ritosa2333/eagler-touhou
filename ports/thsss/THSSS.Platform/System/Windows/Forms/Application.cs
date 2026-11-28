namespace System.Windows.Forms
{
	public static class Application
	{
		public static string StartupPath
		{
			get
			{
				return Environment.CurrentDirectory;
			}
		}

		public static void EnableVisualStyles()
		{
		}

		public static void SetCompatibleTextRenderingDefault(bool defaultValue)
		{
		}

		public static void ExitThread()
		{
		}

		public static void Exit()
		{
		}
	}

	public static class MessageBox
	{
		public static DialogResult Show(string text)
		{
			Console.WriteLine("[MessageBox] " + text);
			return DialogResult.OK;
		}

		public static DialogResult Show(string text, string caption)
		{
			Console.WriteLine("[MessageBox] " + caption + ": " + text);
			return DialogResult.OK;
		}

		public static DialogResult Show(string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon)
		{
			Console.WriteLine("[MessageBox] " + caption + ": " + text);
			return DialogResult.OK;
		}
	}
}
