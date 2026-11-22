using System.Drawing;

namespace Shooting;

public class Item_Line_Gray : Item_Line
{
	public Item_Line_Gray(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "GrazeNum_";
	}

	public override void Ctrl()
	{
		base.Ctrl();
		MaxValue = 0;
		Value = 0;
		Text = MyPlane.Graze.ToString("N0");
		Text = Text.PadLeft(11, ' ');
	}
}
