using System;
using System.Collections.Generic;

namespace SlimDX.DirectInput
{
	public class Joystick : ComObject
	{
		public Joystick(DirectInput directInput, Guid guid)
		{
		}

		public void SetCooperativeLevel(IntPtr handle, CooperativeLevel level)
		{
		}

		public IEnumerable<DeviceObjectInstance> GetObjects()
		{
			return new DeviceObjectInstance[0];
		}

		public ObjectProperties GetObjectPropertiesById(int id)
		{
			return new ObjectProperties();
		}

		public Result Acquire()
		{
			return Result.Success;
		}

		public Result Poll()
		{
			return Result.Success;
		}

		public JoystickState GetCurrentState()
		{
			Result.Last = Result.Success;
			return new JoystickState();
		}

		public void Unacquire()
		{
		}
	}
}
