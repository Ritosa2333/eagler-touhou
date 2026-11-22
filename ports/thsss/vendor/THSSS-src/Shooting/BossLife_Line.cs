using System.Drawing;

namespace Shooting;

public class BossLife_Line : Life_Line
{
	public BossLife_Line(StageDataPackage StageData, string textureName, PointF OriginalPosition)
		: base(StageData, textureName, OriginalPosition)
	{
		TransparentVelocity = 0f;
		TransparentValueF = 0f;
		maxLife = 10;
	}

	public override void Ctrl()
	{
		if (Boss == null)
		{
			return;
		}
		base.Ctrl();
		if (MyPlane.OriginalPosition.X < (float)Life * size && MyPlane.OriginalPosition.Y < size + OriginalPosition.Y)
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
		Life = Boss.Life;
	}

	public override void Show()
	{
		if (Boss == null)
		{
			return;
		}
		size = 16f;
		SizeF destinationSize = new SizeF(size, size);
		for (int i = 0; i < Life - 1; i++)
		{
			if (i < 5)
			{
				PointF position = new PointF(OriginalPosition.X + (float)i * size, OriginalPosition.Y);
				SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(0f, 0f), 0f, position, Color.FromArgb(TransparentValue, Color.White));
			}
			else
			{
				PointF position2 = new PointF(OriginalPosition.X + (float)(i - 5) * size, OriginalPosition.Y + 16f);
				SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(0f, 0f), 0f, position2, Color.FromArgb(TransparentValue, Color.White));
			}
		}
	}
}
