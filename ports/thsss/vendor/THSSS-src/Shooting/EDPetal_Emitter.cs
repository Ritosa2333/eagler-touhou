using System;
using System.Drawing;

namespace Shooting;

public class EDPetal_Emitter : BaseEffect
{
	private string EDText;

	public int Hold { get; set; }

	public EDPetal_Emitter(StageDataPackage StageData, PointF OriginalPosition, string EDText)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		base.OriginalPosition = OriginalPosition;
		LifeTime = 130;
		this.EDText = EDText;
	}

	public override void Shoot()
	{
		if (Time == 10)
		{
			new BaseEffect(StageData, EDText, OriginalPosition, 0f, 0.0)
			{
				Scale = Scale,
				Direction = Math.PI / 2.0,
				OriginalPosition = OriginalPosition,
				Active = false,
				TransparentValueF = 0f,
				TransparentVelocity = 10f,
				LifeTime = 322 + Hold,
				TransparentVelocityDictionary = { 
				{
					300 + Hold,
					-12f
				} }
			};
		}
		for (int i = 0; i < 5; i++)
		{
			if (Time == 1 + i * 3)
			{
				new EDPetal(StageData, "ED_Petal", OriginalPosition, 0f, 0.0)
				{
					DestAngleDegree = 72 * i,
					LifeTime = 300 + Hold
				};
			}
		}
	}
}
