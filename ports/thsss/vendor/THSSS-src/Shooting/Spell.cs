using System;
using System.Drawing;

namespace Shooting;

public class Spell : BaseSpell
{
	public Spell(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Position = MyPlane.Position;
		Damage = 10;
		LifeTime = 300;
		SpellList.Add(this);
		StageData.VibrateStart(LifeTime);
		new BackgroundBlack(StageData, Position);
	}

	public override void Move()
	{
		Position = MyPlane.Position;
	}

	public override bool HitCheck(BaseObject Enemy)
	{
		if (Enemy.Position.Y < Position.Y + 5f && Position.X - 140f < Enemy.Position.X && Enemy.Position.X < Position.X + 140f)
		{
			return true;
		}
		return false;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time % 4 == 0)
		{
			int num = 20;
			float num2 = 2f;
			for (int i = 0; i < num; i++)
			{
				new ParticleColored(StageData, "光点", new PointF(Position.X - (float)num + (float)(2 * i), Position.Y - 20f), 20f, -Math.PI / 2.0 - (double)(num2 / 2f) + (double)((float)i / (float)num * num2));
			}
		}
		if (MyPlane.Position.Y < (float)BoundRect.Bottom)
		{
			MyPlane.Position = new PointF(MyPlane.Position.X, MyPlane.Position.Y + 2f);
		}
	}

	public override void Show()
	{
	}
}
