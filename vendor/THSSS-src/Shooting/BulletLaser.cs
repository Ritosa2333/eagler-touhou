using System;
using System.Drawing;

namespace Shooting;

internal class BulletLaser : BaseBullet
{
	public float DesScaleWidth { get; set; }

	public float DesScaleLength { get; set; }

	public BulletLaser(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		ScaleLength = 0.1f;
		DesScaleLength = 3f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (ScaleLength <= DesScaleLength)
		{
			ScaleLength += 0.02f;
		}
	}

	public override bool HitCheck(BaseObject MyPlane)
	{
		double distance = GetDistance(MyPlane);
		double num = GetDirection(MyPlane);
		double num2 = distance * Math.Cos((float)(Direction - num));
		double num3 = distance * Math.Sin((float)(Direction - num));
		double num4 = (float)Region * ScaleWidth;
		double num5 = (float)TxtureObject.Height * ScaleLength * 0.45f;
		if (num3 * num3 / num4 / num4 + (num2 - (double)((float)TxtureObject.Height * ScaleLength / 2f)) * (num2 - (double)((float)TxtureObject.Height * ScaleLength / 2f)) / num5 / num5 < 1.0)
		{
			return true;
		}
		return false;
	}

	public override void GiveItems()
	{
	}

	public override void Show()
	{
		if (TxtureObject != null)
		{
			SizeF destinationSize = new SizeF((float)TxtureObject.Width * ScaleWidth, (float)TxtureObject.Height * ScaleLength);
			SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(TxtureObject.Width / 2, 0f), (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.FromArgb(TransparentValue, ColorValue));
		}
	}

	public override void ShowRegion()
	{
		PointF originalPosition = OriginalPosition;
		OriginalPosition = new PointF(OriginalPosition.X + (float)TxtureObject.Height * ScaleLength * 0.5f * (float)Math.Cos(Direction), OriginalPosition.Y + (float)TxtureObject.Height * ScaleLength * 0.5f * (float)Math.Sin(Direction));
		SpriteMain.Draw2D(TextureObjectDictionary["Region"], 2f * (float)Region / (float)TextureObjectDictionary["Region"].Width * ScaleWidth, (float)TxtureObject.Height * 0.9f / (float)TextureObjectDictionary["Region"].Height * ScaleLength, (float)(Direction - Math.PI / 2.0 + Angle), Position, Color.White, Mirrored);
		OriginalPosition = originalPosition;
	}
}
