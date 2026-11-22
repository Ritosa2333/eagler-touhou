using System.Drawing;

namespace Shooting;

public class SpellClock : BaseObject
{
	private int timeRemaining;

	public int TimeRemaining
	{
		get
		{
			return timeRemaining;
		}
		set
		{
			if (value < 0)
			{
				timeRemaining = 0;
			}
			else if (value > 5940)
			{
				timeRemaining = 5940;
			}
			else
			{
				timeRemaining = value;
			}
		}
	}

	public SpellClock(StageDataPackage StageData)
	{
		base.StageData = StageData;
		OriginalPosition = new PointF(BoundRect.Left + 25, 40f);
	}

	public override void Ctrl()
	{
		if (Boss == null)
		{
			return;
		}
		base.Ctrl();
		TimeRemaining = Boss.SpellTime - Boss.Time;
		if (Boss.Life > 0)
		{
			if (TimeRemaining <= 300)
			{
				ColorValue = Color.FromArgb(255, 100, 100);
				if (0 < TimeRemaining && TimeRemaining % 60 == 0)
				{
					if (TimeRemaining <= 120)
					{
						StageData.SoundPlay("se_timeout2.wav");
					}
					else
					{
						StageData.SoundPlay("se_timeout.wav");
					}
				}
			}
			else if (TimeRemaining <= 600)
			{
				ColorValue = Color.FromArgb(255, 200, 200);
			}
			else
			{
				ColorValue = Color.White;
			}
		}
		if (MyPlane.OriginalPosition.Y < 30f && MyPlane.OriginalPosition.X < OriginalPosition.X + 60f && MyPlane.OriginalPosition.X > OriginalPosition.X - 60f)
		{
			TransparentValueF -= 10f;
			if (TransparentValueF < 50f)
			{
				TransparentValueF = 50f;
			}
		}
		else
		{
			TransparentValueF += 10f;
		}
	}

	public override void Show()
	{
		if (Boss == null || Boss.Life <= 0)
		{
			return;
		}
		SpriteMain.Draw2D(TextureObjectDictionary["球球"], 1f, 0f, OriginalPosition, Color.FromArgb(TransparentValue, Color.White));
		string text = "tsnum";
		int num = 3;
		int num2 = 9;
		int num3 = 0;
		char[] array = ((float)TimeRemaining / 60f).ToString("F1").ToCharArray();
		for (int num4 = array.Length - 1; num4 >= 0; num4--)
		{
			if (array[num4] == '.')
			{
				num2 = 10;
				text = "tnum";
				num3 = 4;
			}
			PointF position = new PointF(OriginalPosition.X + 18f - (float)((22 - num2) * (array.Length - 1 - num4)) + (float)num3, OriginalPosition.Y + (float)num);
			SpriteMain.Draw2D(TextureObjectDictionary[text + array[num4]], Scale, 0f, position, Color.FromArgb(TransparentValue, ColorValue));
			if (array[num4] == '.')
			{
				num = 0;
				num2 = 3;
				num3 = 20;
			}
		}
	}
}
