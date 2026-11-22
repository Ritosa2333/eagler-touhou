using System.Drawing;

namespace Shooting;

public class Item_Line_Score : Item_Line
{
	private long scoreValue { get; set; }

	public Item_Line_Score(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "ScoreNum_";
	}

	public override void Show()
	{
		MaxValue = 0;
		Value = 0;
		if (MyPlane.Score - scoreValue < 1000)
		{
			scoreValue += 1000L;
		}
		else
		{
			scoreValue += (MyPlane.Score - scoreValue) / 5;
		}
		if (scoreValue > MyPlane.Score)
		{
			scoreValue = MyPlane.Score;
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
