using System.Drawing;

namespace Shooting;

internal class SpellCardBonus : BaseEffect
{
	private PointF ValuePoint;

	private int Index;

	private char[] BonusCharArray { get; set; }

	public SpellCardBonus(StageDataPackage StageData, long Bonus)
		: base(StageData)
	{
		Layer = 1;
		LifeTime = 150;
		TxtureObject = TextureObjectDictionary["GetSpellCardBonus"];
		MyPlane.Score += Bonus;
		TransparentVelocity = 100f;
		TransparentValueF = 0f;
		OriginalPosition = new PointF(BoundRect.Width / 2, 120f);
		AngleDegree = 90.0;
		BonusCharArray = Bonus.ToString().ToCharArray();
		int num = 0;
		for (int i = 0; i < BonusCharArray.Length; i++)
		{
			num += TextureObjectDictionary["Num" + BonusCharArray[i]].Width;
		}
		ValuePoint = new PointF(OriginalPosition.X - (float)(num / 2), OriginalPosition.Y + 40f);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 100)
		{
			TransparentVelocity = -10f;
		}
		if (Time >= 20 && Time % 4 == 0 && Index < BonusCharArray.Length)
		{
			ValuePoint = new PointF(ValuePoint.X + (float)(TextureObjectDictionary["Num" + BonusCharArray[Index]].Width / 2), ValuePoint.Y);
			new ParticleBonusNum(StageData, "Num" + BonusCharArray[Index], ValuePoint).ColorValue = Color.FromArgb(255 - Index * 10, 255 - Index * 5, 255 - Index * 10);
			ValuePoint = new PointF(ValuePoint.X + (float)(TextureObjectDictionary["Num" + BonusCharArray[Index]].Width / 2), ValuePoint.Y);
			Index++;
		}
	}
}
