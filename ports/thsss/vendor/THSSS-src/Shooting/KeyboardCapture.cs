using System;
using System.Windows.Forms;
using SlimDX;
using SlimDX.DirectInput;

namespace Shooting;

public class KeyboardCapture
{
	private Keyboard keyboard;

	public KeyboardState currentKeyboardState;

	public KeyboardState lastKeyboardState;

	public KeyboardCapture(IntPtr handle)
	{
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_000c: Expected Obj, but got Unknown
		//IL_000e: Unknown result type (might be due to invalid IL or missing references)
		//IL_002c: Expected Obj, but got Unknown
		//IL_003c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0011: Unknown result type (might be due to invalid IL or missing references)
		//IL_001b: Expected Obj, but got Unknown
		//IL_0022: Unknown result type (might be due to invalid IL or missing references)
		//IL_0023: Unknown result type (might be due to invalid IL or missing references)
		//IL_004a: Unknown result type (might be due to invalid IL or missing references)
		DirectInput val = new DirectInput();
		CooperativeLevel val2 = (CooperativeLevel)21;
		try
		{
			keyboard = new Keyboard(val);
			((Device)keyboard).SetCooperativeLevel(handle, val2);
		}
		catch (DirectInputException ex)
		{
			DirectInputException ex2 = ex;
			MessageBox.Show("DirectInput Error " + ((Exception)(object)ex2).Message);
			return;
		}
		((Device)keyboard).Acquire();
	}

	public void UpdateInput(ref KeyClass KClass)
	{
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_000b: Unknown result type (might be due to invalid IL or missing references)
		//IL_001c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0021: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Unknown result type (might be due to invalid IL or missing references)
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		Result val = ((Device)keyboard).Acquire();
		if (val.IsFailure)
		{
			return;
		}
		val = ((Device)keyboard).Poll();
		if (!val.IsFailure)
		{
			lastKeyboardState = currentKeyboardState;
			currentKeyboardState = keyboard.GetCurrentState();
			val = Result.Last;
			if (!val.IsFailure)
			{
				KClass.ArrowLeft = currentKeyboardState.IsPressed((Key)76);
				KClass.ArrowRight = currentKeyboardState.IsPressed((Key)118);
				KClass.ArrowUp = currentKeyboardState.IsPressed((Key)132);
				KClass.ArrowDown = currentKeyboardState.IsPressed((Key)50);
				KClass.Key_Shift = currentKeyboardState.IsPressed((Key)78);
				KClass.Key_Z = currentKeyboardState.IsPressed((Key)35) | currentKeyboardState.IsPressed((Key)117);
				KClass.Key_X = currentKeyboardState.IsPressed((Key)33);
				KClass.Key_C = currentKeyboardState.IsPressed((Key)27);
				KClass.Key_Ctrl = currentKeyboardState.IsPressed((Key)75);
				KClass.Key_ESC = currentKeyboardState.IsPressed((Key)53);
				KClass.Key_plus = currentKeyboardState.IsPressed((Key)52);
				KClass.Key_minus = currentKeyboardState.IsPressed((Key)83);
				KClass.Key_plus |= KClass.Key_Ctrl;
				KClass.Key_minus |= KClass.Key_Shift;
			}
		}
	}

	public void Dispose()
	{
		//IL_000e: Unknown result type (might be due to invalid IL or missing references)
		if (keyboard != null)
		{
			((Device)keyboard).Unacquire();
			((ComObject)keyboard).Dispose();
		}
		keyboard = null;
	}
}
