using System;
using System.Drawing;

namespace Shooting;

public class BaseMyPlane_Touhou : BaseMyPlane
{
	public float IndexX { get; set; }

	public int IndexY { get; set; }

	public BaseMyPlane_Touhou(StageDataPackage StageData, Point OriginalPosition, string Name)
	{
		base.StageData = StageData;
		base.OriginalPosition = OriginalPosition;
		base.Name = Name;
		Region = 1;
		Time = UnmatchedTime;
		DeadTime = -1;
		SubPlanePoint = new PointF[4];
		string key = Name + IndexY + (int)IndexX;
		TxtureObject = TextureObjectDictionary[key];
		SpellEnabled = true;
		HighItemScore = 10000;
	}

	public override void TextureCtrl()
	{
		int indexY = IndexY;
		if (Vx > 0.5f)
		{
			IndexY = 2;
			if (IndexX < 4f)
			{
				IndexX += 0.5f;
			}
			else
			{
				IndexX = 4 + Time % 24 / 6;
			}
		}
		else if (Vx < -0.5f)
		{
			IndexY = 1;
			if (IndexX < 4f)
			{
				IndexX += 0.5f;
			}
			else
			{
				IndexX = 4 + Time % 24 / 6;
			}
		}
		else
		{
			IndexY = 0;
			IndexX = Time % 48 / 6;
		}
		if (indexY != IndexY)
		{
			IndexX = 1f;
		}
		if (IndexX > 7f)
		{
			IndexX = 7f;
		}
		string key = Name + IndexY + (int)IndexX;
		TxtureObject = TextureObjectDictionary[key];
	}

	public override void ShowCenter()
	{
		if (SlowMove)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["SlowEffect01"], 1f, (0f - (float)Direction) * 4f, new PointF(Position.X, Position.Y), 120);
			SpriteMain.Draw2D(TextureObjectDictionary["SlowEffect01"], 1f, (float)Direction * 4f, new PointF(Position.X, Position.Y), 120);
			SpriteMain.Draw2D(TextureObjectDictionary["Center"], 1f, (float)Direction * 4f, new PointF(Position.X, Position.Y), Color.White);
		}
	}

	public override void Miss()
	{
		double num = -2.2707963148739676;
		for (int i = 0; i < 8; i++)
		{
			PointF originalPosition = new PointF((float)((double)OriginalPosition.X + 40.0 * Math.Cos(num)), (float)((double)OriginalPosition.Y + 40.0 * Math.Sin(num)));
			new PowerItem_Touhou(StageData, originalPosition)
			{
				Direction = num,
				Velocity = 5f
			};
			num += 0.20000000298023224;
		}
		base.Miss();
	}
}
