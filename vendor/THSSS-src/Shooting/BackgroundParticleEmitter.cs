using System;
using System.Drawing;

namespace Shooting;

internal class BackgroundParticleEmitter : Background3DObject
{
	public BackgroundParticleEmitter(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, OriginalPosition, InsertAtFirst: true)
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time % 30 == 0)
		{
			new PointF(OriginalPosition.X + (float)Ran.Next(-20, 20), OriginalPosition.Y + (float)Ran.Next(-20, 20));
			new Background3DObject(StageData, "591-1", OriginalPosition, InsertAtFirst: false)
			{
				Scale = (float)Ran.Next(40, 60) / 100f,
				ScaleVelocity = -0.004f,
				Velocity = (float)Ran.Next(-10, 10) / 20f,
				DirectionDegree = Ran.Next(360),
				VelocityZ = (float)Ran.Next(6, 15) / 20f,
				LifeTime = 200,
				Active = true,
				Angle3DX = -(float)Math.PI / 2f + StageData.Background3D.Envi.CameraAngle,
				TransparentValueF = 0f,
				TransparentVelocity = 10f
			};
		}
	}

	public override void Show()
	{
	}
}
