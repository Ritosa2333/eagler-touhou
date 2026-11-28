using System;
using System.Threading.Tasks;

namespace Thsss.Browser
{
	public static class Program
	{
		public static async Task Main(string[] args)
		{
			try
			{
				await GameHost.Start();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex);
				try
				{
					ThsssJs.SetStatus(ex.ToString());
				}
				catch
				{
				}
			}
		}
	}
}
