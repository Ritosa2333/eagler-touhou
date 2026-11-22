using System.Windows.Forms;

namespace SlimDX.Windows
{
	public delegate void MainLoop();

	public static class MessagePump
	{
		public static MainLoop Loop { get; private set; }

		public static void Run(Form form, MainLoop loop)
		{
			Loop = loop;
		}
	}
}
