using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

public class BackgroundWing : BaseParticle3D
{
	private int lastlife;

	public BackgroundWing(StageDataPackage StageData, string textureName, PointF OriginalPosition, float Velocity, double Direction, int LifeTime)
		: base(StageData, textureName, OriginalPosition, Velocity, Direction)
	{
		//IL_006b: Unknown result type (might be due to invalid IL or missing references)
		StageData.Particle3D.ParticleList.Add(this);
		base.LifeTime = LifeTime;
		if (Boss != null)
		{
			SetBinding(Boss);
		}
		Scale = 0.1f;
		MaxScale = 0.6f;
		ScaleVelocity = 0.02f;
		RotatingAxis = new Vector3(0f, 1f, 0f);
		lastlife = Boss.Life;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Angle3D = 0f - (float)(Math.PI / 18.0 * (1.0 - Math.Sin((float)Time / 30f)));
		if (Boss != null)
		{
			if (Boss.Time > Boss.UnmatchedTime && Time > 50 && Boss.HitEnabled)
			{
				for (int num = MyBulletList.Count - 1; num >= 0; num--)
				{
					if (MyBulletList[num].OriginalPosition.Y < OriginalPosition.Y && MyBulletList[num].OriginalPosition.X > OriginalPosition.X - 100f && MyBulletList[num].OriginalPosition.X < OriginalPosition.X + 100f)
					{
						if (Boss.Time > Boss.UnmatchedTime)
						{
							Boss.HealthPoint -= (float)MyBulletList[num].Damage * (1f - Boss.Armon) / 3f;
						}
						MyBulletList[num].GiveEndEffect();
						MyBulletList.RemoveAt(num);
						MyPlane.Score += 100L;
					}
				}
			}
			if (Boss.Life < lastlife)
			{
				LifeTime = Time + 10;
				TransparentVelocity = -25f;
				lastlife = Boss.Life;
			}
		}
		if (Time == LifeTime)
		{
			BulletPicEmitter.ShootItem(StageData, "WingItem.png", OriginalPosition, 10f);
		}
	}
}
