using System;
using System.Drawing;

namespace Shooting;

internal class ParticleColored : BaseEffect
{
	private Color color;

	public ParticleColored(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		switch (Ran.Next(7))
		{
		case 0:
			color = Color.BlueViolet;
			break;
		case 1:
			color = Color.CadetBlue;
			break;
		case 2:
			color = Color.DarkBlue;
			break;
		case 3:
			color = Color.Green;
			break;
		case 4:
			color = Color.SkyBlue;
			break;
		case 5:
			color = Color.Blue;
			break;
		case 6:
			color = Color.CornflowerBlue;
			break;
		default:
			color = Color.White;
			break;
		}
		TransparentValueF = 150f;
		Scale = 2f;
		LifeTime = 50;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Direction > Math.PI)
		{
			double num = Math.PI / 2.0 + Direction;
			if (num > Math.PI)
			{
				num -= Math.PI * 2.0;
			}
			Direction -= num / 5.0;
		}
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF((float)TxtureObject.Width * Scale, (float)TxtureObject.Height * Scale);
		SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, TxtureObject.RotatingCenter, 0f, Position, Color.FromArgb(TransparentValue, color));
	}
}
