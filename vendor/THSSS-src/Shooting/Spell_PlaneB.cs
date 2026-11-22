using System;
using System.Drawing;

namespace Shooting;

public class Spell_PlaneB : BaseSpell
{
	public Spell_PlaneB(StageDataPackage StageData)
	{
		base.StageData = StageData;
		OriginalPosition = MyPlane.OriginalPosition;
		Damage = 45;
		Region = 30;
		LifeTime = 390;
		Direction = Math.PI / 2.0;
		SpellList.Add(this);
		new BackgroundBlack(StageData, Position).LifeTime = 410;
		MyPlane.HighSpeed = 1.6f;
		MyPlane.LowSpeed = 0.8f;
		new EmitterSaveEnegy3(StageData, Position).SetBinding(this);
		StageData.SoundPlay("boss_change.wav");
	}

	public override void Move()
	{
		OriginalPosition = MyPlane.OriginalPosition;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time >= 90)
		{
			if (Time == 90)
			{
				StageData.SoundPlay("se_nep00.wav");
				StageData.SoundPlay("se_spell.wav");
				StageData.VibrateStart(300);
				PointF originalPosition = new PointF(OriginalPosition.X, OriginalPosition.Y + 70f);
				PlaneSpark planeSpark = new PlaneSpark(StageData, "Master Spark", originalPosition, 0f, -Math.PI / 2.0);
				planeSpark.MaxScaleW = 1f;
				planeSpark.MaxScaleL = 3f;
				planeSpark.SetBinding(this);
				PlaneSpark planeSpark2 = new PlaneSpark(StageData, "Master Spark", originalPosition, 0f, -Math.PI / 2.0);
				planeSpark2.MaxScaleW = 1.6f;
				planeSpark2.MaxScaleL = 3f;
				planeSpark2.SetBinding(this);
				PlaneSpark planeSpark3 = new PlaneSpark(StageData, "Master Spark", originalPosition, 0f, -Math.PI / 2.0);
				planeSpark3.MaxScaleW = 2.2f;
				planeSpark3.MaxScaleL = 3f;
				planeSpark3.SetBinding(this);
			}
			if (Time == LifeTime - 30)
			{
				Damage = 20;
			}
			if (MyPlane.Position.Y < (float)BoundRect.Bottom)
			{
				MyPlane.Position = new PointF(MyPlane.Position.X, MyPlane.Position.Y + 8f);
			}
		}
	}

	public override bool OutBoundary()
	{
		bool flag = base.OutBoundary();
		if (flag)
		{
			MyPlane.HighSpeed = 5f;
			MyPlane.LowSpeed = 2f;
		}
		return flag;
	}

	public override bool HitCheck(BaseObject Target)
	{
		if (Target.OriginalPosition.Y < OriginalPosition.Y + 5f && OriginalPosition.X - 120f < Target.OriginalPosition.X && Target.OriginalPosition.X < OriginalPosition.X + 120f)
		{
			return true;
		}
		return false;
	}

	public override void HitCheckAll()
	{
		if (Time >= 90)
		{
			base.HitCheckAll();
		}
	}
}
