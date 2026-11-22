using System;
using System.Drawing;

namespace Shooting;

internal class SpellCardAttack : BaseEffect
{
	public SpellCardAttack(StageDataPackage StageData)
		: base(StageData)
	{
		LifeTime = 2;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				PointF originalPosition = new PointF(-50f + (float)(j * 128) * (float)Math.Cos(Math.PI / 6.0), 200f - (float)(j * 128) * (float)Math.Sin(Math.PI / 6.0) + (float)(i * 35));
				BaseObject baseObject = new BaseObject(StageData, "SpellCircle06", new PointF(0f, 0f), 2f, -Math.PI / 6.0)
				{
					OriginalPosition = originalPosition,
					Angle = -Math.PI,
					LifeTime = 50
				};
				baseObject.TransparentVelocityDictionary.Add(30, -13f);
				if (i % 2 == 0)
				{
					baseObject.Velocity *= -1f;
				}
				Background2.BackgroundList.Add(baseObject);
			}
		}
	}
}
