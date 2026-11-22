using System;
using System.Drawing;

namespace Shooting;

public class BossMark : BaseObject
{
	public BossMark(StageDataPackage StageData)
	{
		base.StageData = StageData;
		TxtureObject = TextureObjectDictionary["BossMark"];
		Position = new PointF(BoundRect.Left + BoundRect.Width / 2, 472f);
		AngleDegree = 90.0;
	}

	public override void Ctrl()
	{
		Time++;
		if (Boss != null)
		{
			OriginalPosition = new PointF(Boss.OriginalPosition.X + (float)BoundRect.X, 472f);
			TransparentValueF = 150 + (int)(100.0 * Math.Sin((double)Time * Math.PI * 2.0 / 60.0));
		}
	}

	public override void Show()
	{
		if (Boss != null && TxtureObject != null)
		{
			SpriteMain.Draw2D(TxtureObject, ScaleWidth, ScaleLength, (float)(Direction - Math.PI / 2.0 + Angle), OriginalPosition, Color.FromArgb(TransparentValue, ColorValue), Mirrored);
		}
	}
}
