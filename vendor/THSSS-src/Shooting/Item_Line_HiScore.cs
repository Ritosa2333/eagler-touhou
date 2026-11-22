using System.Drawing;

namespace Shooting;

public class Item_Line_HiScore : Item_Line
{
	private long scoreValue { get; set; }

	public Item_Line_HiScore(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "HiScoreNum_";
	}

	public override void Show()
	{
		MaxValue = 0;
		Value = 0;
		if (MyPlane.HiScore - scoreValue < 1000)
		{
			scoreValue += 1000L;
		}
		else
		{
			scoreValue += (MyPlane.HiScore - scoreValue) / 5;
		}
		if (scoreValue > MyPlane.HiScore)
		{
			scoreValue = MyPlane.HiScore;
		}
		if (scoreValue < 10000000000L)
		{
			Text = (10000000000L + scoreValue).ToString("N0");
			Text = Text.Remove(0, 1).PadLeft(10, ' ');
		}
		else
		{
			Text = scoreValue.ToString("N0");
			OriginalPosition = new PointF(468 - Delta, OriginalPosition.Y);
		}
		base.Show();
	}
}
