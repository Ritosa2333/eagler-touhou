namespace Shooting;

public class KeyClass
{
	public bool ArrowUp;

	public bool ArrowDown;

	public bool ArrowLeft;

	public bool ArrowRight;

	public bool Key_Shift;

	public bool Key_Z;

	public bool Key_X;

	public bool Key_C;

	public bool Key_Ctrl;

	public bool Key_ESC;

	public bool Key_plus;

	public bool Key_minus;

	public int Key2Hex()
	{
		int num = 0;
		if (ArrowUp)
		{
			num++;
		}
		num <<= 1;
		if (ArrowDown)
		{
			num++;
		}
		num <<= 1;
		if (ArrowLeft)
		{
			num++;
		}
		num <<= 1;
		if (ArrowRight)
		{
			num++;
		}
		num <<= 1;
		if (Key_Shift)
		{
			num++;
		}
		num <<= 1;
		if (Key_Z)
		{
			num++;
		}
		num <<= 1;
		if (Key_X)
		{
			num++;
		}
		num <<= 1;
		if (Key_C)
		{
			num++;
		}
		num <<= 1;
		if (Key_Ctrl)
		{
			num++;
		}
		num <<= 1;
		return num << 6;
	}

	public void Hex2Key(int keyValue)
	{
		keyValue >>= 7;
		Key_Ctrl = keyValue % 2 == 1;
		keyValue >>= 1;
		Key_C = keyValue % 2 == 1;
		keyValue >>= 1;
		Key_X = keyValue % 2 == 1;
		keyValue >>= 1;
		Key_Z = keyValue % 2 == 1;
		keyValue >>= 1;
		Key_Shift = keyValue % 2 == 1;
		keyValue >>= 1;
		ArrowRight = keyValue % 2 == 1;
		keyValue >>= 1;
		ArrowLeft = keyValue % 2 == 1;
		keyValue >>= 1;
		ArrowDown = keyValue % 2 == 1;
		keyValue >>= 1;
		ArrowUp = keyValue % 2 == 1;
		keyValue >>= 1;
	}

	public static void KeyMix(ref KeyClass KClass, KeyClass KClass1, KeyClass KClass2)
	{
		KClass.Key_Z = KClass1.Key_Z | KClass2.Key_Z;
		KClass.Key_X = KClass1.Key_X | KClass2.Key_X;
		KClass.Key_C = KClass1.Key_C | KClass2.Key_C;
		KClass.Key_Shift = KClass1.Key_Shift | KClass2.Key_Shift;
		KClass.Key_Ctrl = KClass1.Key_Ctrl | KClass2.Key_Ctrl;
		KClass.ArrowLeft = KClass1.ArrowLeft | KClass2.ArrowLeft;
		KClass.ArrowRight = KClass1.ArrowRight | KClass2.ArrowRight;
		KClass.ArrowDown = KClass1.ArrowDown | KClass2.ArrowDown;
		KClass.ArrowUp = KClass1.ArrowUp | KClass2.ArrowUp;
		KClass.Key_ESC = KClass1.Key_ESC | KClass2.Key_ESC;
		KClass.Key_plus = KClass1.Key_plus | KClass2.Key_plus;
		KClass.Key_minus = KClass1.Key_minus | KClass2.Key_minus;
	}
}
