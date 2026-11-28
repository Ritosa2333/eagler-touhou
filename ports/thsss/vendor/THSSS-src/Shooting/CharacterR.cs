using System.Drawing;

namespace Shooting;

internal class CharacterR : BaseStoryItem
{
	public CharacterR(StageDataPackage StageData, string textureName)
		: base(StageData)
	{
		Position = new PointF(BoundRect.Right - 100, BoundRect.Top + 200);
		Velocity = 0f;
		Direction = 0.0;
		TxtureObject = TextureObjectDictionary[textureName];
		Active = false;
		TransparentValueF = 0f;
	}

	public override void Move()
	{
		if (Active)
		{
			if (Position.X > (float)(BoundRect.Right - 130))
			{
				OriginalPosition = new PointF(OriginalPosition.X - 2f, OriginalPosition.Y);
			}
		}
		else if (Position.X < (float)(BoundRect.Right - 100))
		{
			OriginalPosition = new PointF(OriginalPosition.X + 2f, OriginalPosition.Y);
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time <= 1)
		{
			TransparentValueF = 0f;
		}
		else if (Time < 26)
		{
			TransparentValueF += 10f;
		}
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF((float)TxtureObject.Width * Scale, (float)TxtureObject.Height * Scale);
		if (Active)
		{
			SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, TxtureObject.RotatingCenter, 0f, Position, Color.FromArgb(TransparentValue, Color.White));
		}
		else
		{
			SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, TxtureObject.RotatingCenter, 0f, Position, Color.FromArgb(TransparentValue, Color.Gray));
		}
	}
}
