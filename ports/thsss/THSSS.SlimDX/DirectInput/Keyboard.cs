using System;

namespace SlimDX.DirectInput
{
	public class Keyboard : ComObject
	{
		public Keyboard(DirectInput directInput)
		{
		}

		public void SetCooperativeLevel(IntPtr handle, CooperativeLevel level)
		{
		}

		public Result Acquire()
		{
			var result = Result.Success;
			Result.Last = result;
			return result;
		}

		public Result Poll()
		{
			var result = Result.Success;
			Result.Last = result;
			return result;
		}

		public KeyboardState GetCurrentState()
		{
			Result.Last = Result.Success;
			return new KeyboardState();
		}

		public void Unacquire()
		{
		}
	}
}
