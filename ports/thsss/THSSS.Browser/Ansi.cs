using System.Text;

namespace Shooting
{
	public static class Ansi
	{
		public static readonly Encoding GBK;

		static Ansi()
		{
			Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
			GBK = Encoding.GetEncoding(936);
		}
	}
}
