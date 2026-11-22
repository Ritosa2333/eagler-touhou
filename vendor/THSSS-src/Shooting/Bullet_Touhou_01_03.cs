using System.Drawing;

namespace Shooting;

public class Bullet_Touhou_01_03 : Bullet_Touhou_01
{
	public string SubBulletName { get; set; }

	public Bullet_Touhou_01_03(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
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
			BaseObject bullet = new BaseObject(StageData, SubBulletName + colorType, new PointF(0f, 0f), 2.5f, 0.0);
			new BulletGroupCircle(StageData, bullet, OriginalPosition, Direction, (int)Difficulty * 2 + 3, 0, colorType);
			GiveEndEffect();
		}
	}
}
