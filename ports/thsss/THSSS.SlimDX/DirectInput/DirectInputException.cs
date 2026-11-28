using System;

namespace SlimDX.DirectInput
{
	public class DirectInputException : Exception
	{
		public DirectInputException(string message)
			: base(message)
		{
		}
	}
}
