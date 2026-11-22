using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class EndBoss_Touhou_Small : EndBoss_Touhou
{
	public EndBoss_Touhou_Small(StageDataPackage StageData, PointF OriginalPosition, Color Color1, Color Color2)
		: base(StageData, OriginalPosition, Color1, Color2)
	{
	}

	public EndBoss_Touhou_Small(StageDataPackage StageData)
		: base(StageData)
	{
	}

	public override void Shoot()
	{
		//IL_0173: Unknown result type (might be due to invalid IL or missing references)
		if (Time == 1)
		{
			StageData.SoundPlay("se_enep01.wav", OriginalPosition.X / (float)BoundRect.Width);
			for (int i = 0; i < 40; i++)
			{
				new Particle3D2(StageData, "Effect-1", OriginalPosition, (float)Ran.Next(50, 70) / 10f, (float)Ran.Next(360) / 180f * (float)Math.PI, 30 + StageData.Ran.Next(50))
				{
					Direction3D = (float)StageData.Ran.Next(0, 90) * (float)Math.PI / 180f,
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
