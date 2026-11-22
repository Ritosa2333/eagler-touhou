using System.Drawing;

namespace Shooting;

public class Bullet_Touhou_01 : BaseBullet_Touhou
{
	public float Velocity0 { get; set; }

	public float Velocity1 { get; set; }

	public int Time1 { get; set; }

	public float Velocity2 { get; set; }

	public int Time2 { get; set; }

	public Bullet_Touhou_01(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction, ColorType)
	{
		Velocity0 = Velocity;
	}

	public override void Ctrl()
	{
		if (Time < Time1)
		{
			Accelerate = (Velocity1 - Velocity0) / (float)Time1;
		}
		else if (Time1 == Time)
		{
			Accelerate = 0f;
		}
		else if (Time1 < Time && Time < Time2)
		{
			Accelerate = (Velocity2 - Velocity1) / (float)(Time2 - Time1);
		}
		else if (Time2 == Time)
		{
			Accelerate = 0f;
		}
		base.Ctrl();
	}
}
