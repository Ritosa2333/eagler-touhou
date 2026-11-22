using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class EndBoss_Touhou : BaseEffect
{
	public int R1;

	public int R2;

	public int G1;

	public int G2;

	public int B1;

	public int B2;

	public EndBoss_Touhou(StageDataPackage StageData, PointF OriginalPosition, Color Color1, Color Color2)
		: this(StageData)
	{
		base.OriginalPosition = OriginalPosition;
		R2 = Math.Max(Color1.R, Color2.R);
		R1 = Math.Min(Color1.R, Color2.R);
		G2 = Math.Max(Color1.G, Color2.G);
		G1 = Math.Min(Color1.G, Color2.G);
		B2 = Math.Max(Color1.B, Color2.B);
		B1 = Math.Min(Color1.B, Color2.B);
	}

	public EndBoss_Touhou(StageDataPackage StageData)
		: base(StageData)
	{
		LifeTime = 250;
	}

	public override void Move()
	{
		if (Boss != null)
		{
			Position = Boss.Position;
		}
	}

	public override void Shoot()
	{
		//IL_0173: Unknown result type (might be due to invalid IL or missing references)
		//IL_035d: Unknown result type (might be due to invalid IL or missing references)
		if (Time == 1)
		{
			StageData.SoundPlay("se_enep01.wav", OriginalPosition.X / (float)BoundRect.Width);
			for (int i = 0; i < 20; i++)
			{
				new Particle3D2(StageData, "Effect-1", OriginalPosition, (float)Ran.Next(10, 80) / 10f, (float)Ran.Next(360) / 180f * (float)Math.PI, 30 + StageData.Ran.Next(50))
				{
					Direction3D = (float)StageData.Ran.Next(0, 30) * (float)Math.PI / 180f,
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10, 18) / 100f,
					Scale = (float)StageData.Ran.Next(30, 40) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					ColorValue = Color.FromArgb(Ran.Next(R1, R2), Ran.Next(G1, G2), Ran.Next(B1, B2))
				};
			}
			StageData.VibrateStart(50);
		}
		else if (Time == 100)
		{
			StageData.SoundPlay("se_enep01.wav", OriginalPosition.X / (float)BoundRect.Width);
			for (int j = 0; j < 80; j++)
			{
				new Particle3D2(StageData, "Effect-1", OriginalPosition, (float)Ran.Next(10, 140) / 10f, (float)Ran.Next(360) / 180f * (float)Math.PI, 30 + StageData.Ran.Next(50))
				{
					Direction3D = (float)StageData.Ran.Next(0, 30) * (float)Math.PI / 180f,
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10, 18) / 100f,
					Scale = (float)StageData.Ran.Next(30, 40) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					ColorValue = Color.FromArgb(Ran.Next(R1, R2), Ran.Next(G1, G2), Ran.Next(B1, B2))
				};
			}
			StageData.VibrateStart(90);
			Boss = null;
		}
	}

	public override void Show()
	{
	}
}
