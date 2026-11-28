using System;
using System.Drawing;

namespace Shooting;

internal class ClearBonus : BaseEffect
{
	private PointF ValuePoint;

	private int Index;

	private char[] BonusCharArray { get; set; }

	public ClearBonus(StageDataPackage StageData, int Bonus)
		: base(StageData)
	{
		Layer = 1;
		LifeTime = 150;
		TxtureObject = TextureObjectDictionary["StageClear"];
		MyPlane.Score += Bonus;
		TransparentVelocity = 6f;
		TransparentValueF = 0f;
		OriginalPosition = new PointF(BoundRect.Width / 2, 150f);
		AngleDegree = 90.0;
		BonusCharArray = Bonus.ToString().ToCharArray();
		int num = 0;
		for (int i = 0; i < BonusCharArray.Length; i++)
		{
			num += TextureObjectDictionary["Num" + BonusCharArray[i]].Width;
		}
		ValuePoint = new PointF(OriginalPosition.X - (float)(num / 2), OriginalPosition.Y + 36f);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 100)
		{
			TransparentVelocity = -6f;
		}
		if (Time == 20)
		{
			while (Index < BonusCharArray.Length)
			{
				ValuePoint = new PointF(ValuePoint.X + (float)(TextureObjectDictionary["Num" + BonusCharArray[Index]].Width / 2), ValuePoint.Y);
				new BaseEffect(StageData, "Num" + BonusCharArray[Index], new PointF(0f, 0f), 0f, Math.PI / 2.0)
				{
					Active = false,
					OriginalPosition = ValuePoint,
					ColorValue = Color.FromArgb(255 - Index * 10, 255 - Index * 5, 255 - Index * 10),
					LifeTime = 100,
					TransparentValueF = 0f,
					TransparentVelocityDictionary = 
					{
						{ 1, 13f },
						{ 80, -13f }
					}
				};
				ValuePoint = new PointF(ValuePoint.X + (float)(TextureObjectDictionary["Num" + BonusCharArray[Index]].Width / 2), ValuePoint.Y);
				Index++;
			}
		}
	}
}
