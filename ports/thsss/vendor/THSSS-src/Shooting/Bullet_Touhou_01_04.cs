using System;
using System.Drawing;

namespace Shooting;

public class Bullet_Touhou_01_04 : Bullet_Touhou_01
{
	public string SubBulletName { get; set; }

	public Bullet_Touhou_01_04(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction, ColorType)
	{
		LifeTime = 50;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == LifeTime)
		{
			byte colorType = ColorType;
			int num = (int)(Difficulty + 4);
			float num2 = -(float)Math.PI / 6f;
			float num3 = (float)Math.PI * -2f / 3f;
			switch ((int)Difficulty)
			{
			case 0:
				num = 2;
				break;
			case 1:
				num = 4;
				break;
			case 2:
				num = 6;
				break;
			case 3:
				num = 7;
				break;
			case 4:
				num = 8;
				break;
			}
			float num4 = num3 / ((float)num - 1f);
			for (int i = 0; i < num; i++)
			{
				new BaseBullet_Touhou(StageData, SubBulletName + colorType, OriginalPosition, 2.5f, num2, colorType)
				{
					Ay = 0.08f,
					MaxVelocity = 2.5f
				};
				num2 += num4;
			}
			StageData.SoundPlay("se_kira01.wav");
			GiveEndEffect();
		}
	}
}
