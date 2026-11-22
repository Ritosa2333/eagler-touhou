using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class EmitterGiveOutEnegy3D : BaseEffect
{
	public EmitterGiveOutEnegy3D(StageDataPackage StageData, PointF OriginalPosition, Color color)
		: base(StageData)
	{
		base.OriginalPosition = OriginalPosition;
		Velocity = 0f;
		Direction = 0.0;
		LifeTime = 25;
		ColorValue = color;
		new BaseEffect(StageData, "Effect-2", Position, 0f, 0.0)
		{
			LifeTime = 100,
			Scale = 1f,
			ScaleVelocity = 0.8f,
			ColorValue = color,
			TransparentValueF = 180f,
			TransparentVelocity = -2f
		};
	}

	public override void Shoot()
	{
		//IL_0148: Unknown result type (might be due to invalid IL or missing references)
		if (Time == 1)
		{
			for (int i = 0; i < 50; i++)
			{
				new Particle3D1(StageData, "Effect-1", OriginalPosition, (float)Ran.Next(50, 130) / 10f, (float)Ran.Next(360) / 180f * (float)Math.PI, 30 + StageData.Ran.Next(50))
				{
					Direction3D = (float)StageData.Ran.Next(0, 60) * (float)Math.PI / 180f,
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10, 18) / 100f,
					Scale = (float)StageData.Ran.Next(20, 30) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					ColorValue = ColorValue
				};
			}
		}
	}

	public override void Show()
	{
	}
}
