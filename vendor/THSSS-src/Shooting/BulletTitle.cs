using System.Drawing;

namespace Shooting;

internal class BulletTitle : MusicTitle
{
	public BulletTitle(StageDataPackage StageData, string textureName)
		: this(StageData, textureName, new Point(0, 0))
	{
		DestPoint = new Point(BoundRect.Width - 10, 20);
	}

	public BulletTitle(StageDataPackage StageData, string textureName, Point DestPoint)
		: base(StageData, textureName, DestPoint)
	{
		Layer = 1;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 200 && Boss != null && Boss.OnSpell && Boss.HealthPoint > 0f)
		{
			LifeTime++;
		}
		if (MyPlane.OriginalPosition.Y < OriginalPosition.Y + (float)TxtureObject.Height + 16f && MyPlane.OriginalPosition.X > OriginalPosition.X - (float)TxtureObject.Width && MyPlane.OriginalPosition.X < OriginalPosition.X)
		{
			TransparentValueF -= 10f;
			if (TransparentValueF < 50f)
			{
				TransparentValueF = 50f;
			}
		}
		else if (Boss != null && Time > 100)
		{
			if (Boss.OriginalPosition.Y < OriginalPosition.Y + (float)TxtureObject.Height + 16f && Boss.OriginalPosition.Y > 0f && Boss.OriginalPosition.X > OriginalPosition.X - (float)TxtureObject.Width && Boss.OriginalPosition.X < OriginalPosition.X)
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
		else
		{
			TransparentValueF += 10f;
		}
	}

	public void DrawText(string Text, Point Pos, float Scale)
	{
		int num = 0;
		char[] array = Text.ToCharArray();
		for (int i = 0; i < array.Length; i++)
		{
			if (array[i] == '.' || array[i] == ',')
			{
				num += (int)(8f * Scale);
			}
		}
		for (int j = 0; j < array.Length; j++)
		{
			if (array[j] == '.' || array[j] == ',')
			{
				num -= (int)(5f * Scale);
			}
			PointF position = new PointF(Pos.X + 5 + num, Pos.Y + 9);
			SpriteMain.Draw2D(TextureObjectDictionary[array[j].ToString() ?? ""], Scale, 0f, position, Color.FromArgb(TransparentValue, Color.White));
			num += (int)(14f * Scale);
			if (array[j] == '.' || array[j] == ',')
			{
				num -= (int)(3f * Scale);
			}
		}
	}

	public override void Show()
	{
		TextureObject textureObject = StageData.TextureObjectDictionary["tiaotiao2"];
		SpriteMain.Draw2D(textureObject.TXTure, textureObject.PosRect, new SizeF((float)textureObject.Width * Scale, (float)textureObject.Height * Scale), textureObject.RightTop, 0f, new PointF(Position.X, Position.Y - 16f), Color.FromArgb(TransparentValue, Color.White));
		base.Show();
		if (StageData.SCBstring != null)
		{
			DrawText(StageData.SCBstring, new Point(240, 68), 0.6f);
		}
	}
}
