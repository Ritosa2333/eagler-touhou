using System;
using System.Drawing;

namespace Shooting;

public class BackgroundYoumu(StageDataPackage StageData, string textureName) : BackgroundRotate(StageData, textureName)
{
	private double[] lastAngle = new double[10];

	private float sc;

	public override void Ctrl()
	{
		base.Ctrl();
		for (int num = lastAngle.Length - 1; num > 0; num--)
		{
			lastAngle[num] = lastAngle[num - 1];
		}
		lastAngle[0] = Angle;
		if (StageData.SlowMode > 0)
		{
			Active = true;
			ColorValue = Color.Pink;
			AngularVelocityDegree = 1f;
		}
		else
		{
			Active = false;
			ColorValue = Color.White;
			sc = (float)Math.Sin((double)((float)Time / 103.5f) * Math.PI * 2.0) / 16f;
			Scale = 2.5f + sc;
			AngularVelocityDegree = -0.5f;
		}
	}

	public override void Show()
	{
		if (StageData.SlowMode > 0)
		{
			double num = Angle;
			int num2 = TransparentValue;
			for (int num3 = lastAngle.Length - 1; num3 >= 0; num3 -= 2)
			{
				Angle = lastAngle[num3];
				TransparentValueF = num2 * (lastAngle.Length - num3 + 15) / (lastAngle.Length + 1 + 15);
				base.Show();
			}
			Angle = num;
			TransparentValueF = num2;
		}
		base.Show();
	}
}
