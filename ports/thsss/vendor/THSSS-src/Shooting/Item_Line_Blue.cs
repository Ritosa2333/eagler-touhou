using System.Drawing;

namespace Shooting;

public class Item_Line_Blue : Item_Line
{
	public Item_Line_Blue(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "BlueNum_";
	}

	public override void Ctrl()
	{
		base.Ctrl();
		MaxValue = 10000;
		Value = 0;
		Text = MyPlane.HighItemScore.ToString("N0");
		Text = Text.PadLeft(11, ' ');
	}
}
