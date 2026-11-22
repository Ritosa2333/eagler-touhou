using System;

namespace SlimDX.DirectInput
{
	[Flags]
	public enum CooperativeLevel
	{
		Exclusive = 1,
		Nonexclusive = 2,
		Foreground = 4,
		Background = 8,
		NoWinKey = 16
	}
}
