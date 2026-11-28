using System;
using System.Drawing;

namespace Shooting;

internal class EnemyPlane_TouhouGhost : BaseEnemyPlane_Touhou
{
	public bool BackFire { get; set; }

	public Color BackFireColor { get; set; }

	public EnemyPlane_TouhouGhost(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, null, OriginalPosition, Velocity, Direction, ColorType)
	{
		HealthPoint = 30f;
		Scale = 0.2f;
		MaxScale = 1f;
		ScaleVelocity = 0.2f;
	}

	public override void GiveItems()
	{
		if (ColorType == 0 || ColorType == 2)
		{
			PointF originalPosition = new PointF(OriginalPosition.X + (float)Ran.Next(-5, 5), OriginalPosition.Y + (float)Ran.Next(-5, 5));
			new PowerItem_Touhou(StageData, originalPosition);
		}
		else if (ColorType == 1 || ColorType == 3)
		{
			new PointF(OriginalPosition.X + (float)Ran.Next(-5, 5), OriginalPosition.Y + (float)Ran.Next(-5, 5));
			new ScoreItem_Touhou(StageData, OriginalPosition);
		}
		if (Difficulty <= DifficultLevel.Normal)
		{
			return;
		}
		int num = ((Difficulty <= DifficultLevel.Hard) ? 1 : 8);
		double num2 = GetDirection(MyPlane);
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				int num3 = ColorType * 3 + 2;
				new Bullet_Touhou_01(StageData, "bullet2_" + num3, OriginalPosition, 1.5f + (float)j * 0.4f, num2, (byte)num3);
			}
			num2 += Math.PI * 2.0 / (double)num;
		}
	}

	public override void Shoot()
	{
	}

	public override void TextureCtrl()
	{
		TxtureObject = TextureObjectDictionary["EnemyGhost" + ColorType + "_" + Time % 32 / 4];
		int maxValue = 6;
		PointF position = new PointF(Position.X + (float)Ran.Next(maxValue) * (float)Math.Sin((double)Ran.Next(360) * Math.PI / 180.0), Position.Y + (float)Ran.Next(maxValue) * (float)Math.Cos((double)Ran.Next(360) * Math.PI / 180.0));
		new BaseEffect(StageData, "EnemyGhost" + ColorType + "_" + Time % 32 / 4, position, 1.5f, -Math.PI / 2.0)
		{
			Scale = 0.5f,
			LifeTime = 13,
			ScaleVelocity = -0.04f
		};
	}
}
