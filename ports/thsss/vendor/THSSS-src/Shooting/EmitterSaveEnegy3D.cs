using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

internal class EmitterSaveEnegy3D : BaseEffect
{
	public EmitterSaveEnegy3D(StageDataPackage StageData, PointF OriginalPosition, Color color)
		: base(StageData)
	{
		base.OriginalPosition = OriginalPosition;
		Velocity = 0f;
		Direction = 0.0;
		LifeTime = 25;
		ColorValue = color;
		new BaseEffect(StageData, "Effect-2", Position, 0f, 0.0)
		{
			LifeTime = 50,
			Scale = 14f,
			ScaleVelocity = -0.25f,
			ColorValue = color,
			TransparentValueF = 0f,
			TransparentVelocity = 6f,
			MaxTransparent = 128
		};
	}

	public override void Shoot()
	{
		//IL_0166: Unknown result type (might be due to invalid IL or missing references)
		if (Time % 2 == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				double num = (double)((float)Ran.Next(360) / 180f) * Math.PI;
				int num2 = Ran.Next(150, 200);
				PointF originalPosition = new PointF((float)((double)OriginalPosition.X + (double)num2 * Math.Cos(num)), (float)((double)OriginalPosition.Y + (double)num2 * Math.Sin(num)));
				Particle3D1 particle3D = new Particle3D1(StageData, "Effect-1", originalPosition, 0f, num + Math.PI, Ran.Next(50, 80))
				{
					Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
					AngularVelocity3D = (float)StageData.Ran.Next(10, 18) / 100f,
					Scale = (float)StageData.Ran.Next(30, 40) / 10f,
					RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
					ColorValue = ColorValue
				};
				particle3D.Velocity = 2f * (float)num2 / (float)particle3D.LifeTime;
				particle3D.Accelerate = (0f - particle3D.Velocity) * particle3D.Velocity / (float)(2 * num2);
				particle3D.LifeTime -= 15;
			}
		}
	}

	public override void Show()
	{
	}
}
