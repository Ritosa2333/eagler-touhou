using System.Drawing;

namespace Shooting;

internal class BackgroundBlack : BaseObject
{
	public BackgroundBlack(StageDataPackage StageData, PointF Position)
		: base(StageData, "MyBullet0008", new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		base.Position = Position;
		Scale = 1f;
		LifeTime = 350;
		TransparentValueF = 0f;
		MaxTransparent = 180;
		ColorValue = Color.Black;
	}

	public override void Ctrl()
	{
		Time++;
		Scale++;
		if (Time < 40)
		{
			TransparentValueF += 6f;
		}
		if (Time > LifeTime - 40)
		{
			TransparentValueF -= 6f;
		}
	}
}
