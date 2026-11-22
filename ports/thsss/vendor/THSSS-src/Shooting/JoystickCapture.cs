using System;
using SlimDX;
using SlimDX.DirectInput;

namespace Shooting;

public class JoystickCapture
{
	private Joystick joystick;

	public JoystickState currentJoystickState = new JoystickState();

	public JoystickState lastJoystickState = new JoystickState();

	public JoystickCapture(IntPtr handle)
	{
		//IL_0001: Unknown result type (might be due to invalid IL or missing references)
		//IL_000b: Expected Obj, but got Unknown
		//IL_000c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0016: Expected Obj, but got Unknown
		//IL_001c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0022: Expected Obj, but got Unknown
		//IL_0041: Unknown result type (might be due to invalid IL or missing references)
		//IL_004b: Expected Obj, but got Unknown
		//IL_0053: Unknown result type (might be due to invalid IL or missing references)
		//IL_008f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0094: Unknown result type (might be due to invalid IL or missing references)
		//IL_0098: Unknown result type (might be due to invalid IL or missing references)
		//IL_009e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b3: Expected I4, but got Unknown
		//IL_00dc: Unknown result type (might be due to invalid IL or missing references)
		DirectInput val = new DirectInput();
		foreach (DeviceInstance device in val.GetDevices((DeviceClass)4, (DeviceEnumerationFlags)1))
		{
			try
			{
				joystick = new Joystick(val, device.InstanceGuid);
				((Device)joystick).SetCooperativeLevel(handle, (CooperativeLevel)5);
			}
			catch (DirectInputException)
			{
				continue;
			}
			break;
		}
		if (joystick == null)
		{
			return;
		}
		foreach (DeviceObjectInstance @object in ((Device)joystick).GetObjects())
		{
			DeviceObjectInstance current2 = @object;
			if ((current2.ObjectType & 3) != 0)
			{
				((Device)joystick).GetObjectPropertiesById((int)current2.ObjectType).SetRange(-1000, 1000);
			}
		}
		((Device)joystick).Acquire();
	}

	public void UpdateInput(ref KeyClass KClass)
	{
		//IL_000f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0014: Unknown result type (might be due to invalid IL or missing references)
		//IL_0025: Unknown result type (might be due to invalid IL or missing references)
		//IL_002a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0052: Unknown result type (might be due to invalid IL or missing references)
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		if (joystick == null)
		{
			return;
		}
		Result val = ((Device)joystick).Acquire();
		if (val.IsFailure)
		{
			return;
		}
		val = ((Device)joystick).Poll();
		if (!val.IsFailure)
		{
			lastJoystickState = currentJoystickState;
			currentJoystickState = joystick.GetCurrentState();
			val = Result.Last;
			if (!val.IsFailure)
			{
				KClass.ArrowLeft = currentJoystickState.X < -600;
				KClass.ArrowRight = currentJoystickState.X > 600;
				KClass.ArrowUp = currentJoystickState.Y < -600;
				KClass.ArrowDown = currentJoystickState.Y > 600;
				KClass.Key_Shift = currentJoystickState.IsPressed(MapJoyStick.Slow);
				KClass.Key_Z = currentJoystickState.IsPressed(MapJoyStick.Shoot);
				KClass.Key_X = currentJoystickState.IsPressed(MapJoyStick.Bomb);
				KClass.Key_Ctrl = currentJoystickState.IsPressed(MapJoyStick.Skip);
				KClass.Key_ESC = currentJoystickState.IsPressed(MapJoyStick.Pause);
				KClass.Key_plus |= KClass.Key_Ctrl;
				KClass.Key_minus |= KClass.Key_Shift;
			}
		}
	}

	public void Dispose()
	{
		//IL_000e: Unknown result type (might be due to invalid IL or missing references)
		if (joystick != null)
		{
			((Device)joystick).Unacquire();
			((ComObject)joystick).Dispose();
		}
		joystick = null;
	}
}
