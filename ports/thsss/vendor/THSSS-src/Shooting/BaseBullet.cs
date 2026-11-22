using System;
using System.Drawing;

namespace Shooting;

public class BaseBullet : BaseBullet_Touhou
{
	private int highLightValue;

	public int HighLightValue
	{
		get
		{
			return highLightValue;
		}
		set
		{
			if (value < 0)
			{
				highLightValue = 0;
			}
			else if (value > 255)
			{
				highLightValue = 255;
			}
			else
			{
				highLightValue = value;
			}
		}
	}

	public BaseBullet(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		HighLightValue = 200;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (HighLightValue > 0)
		{
			HighLightValue -= 10;
		}
		else
		{
			HighLightValue = 0;
		}
	}

	public override void HitCheckAll()
	{
		if (MyPlane.HitEnabled && HitCheck(MyPlane))
		{
			MyPlane.PreMiss();
		}
	}

	public override void GiveEndEffect()
	{
		new ParticleSmaller(StageData, "光点", Position, Velocity, Direction).LifeTime = 30;
	}

	public override void Show()
	{
		base.Show();
		if (HighLightValue > 0)
		{
			int width = TextureObjectDictionary["光点"].Width;
			int height = TextureObjectDictionary["光点"].Height;
			SizeF destinationSize = new SizeF((float)width * Scale, (float)height * Scale);
			SpriteMain.Draw2D(TextureObjectDictionary["光点"].TXTure, TextureObjectDictionary["光点"].PosRect, destinationSize, TextureObjectDictionary["光点"].RotatingCenter, (float)(Direction - Math.PI / 2.0), Position, Color.FromArgb(HighLightValue, Color.White));
		}
	}
}
