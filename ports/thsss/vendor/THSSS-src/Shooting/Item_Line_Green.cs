using System.Drawing;

namespace Shooting;

public class Item_Line_Green : Item_Line
{
	public Item_Line_Green(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		ColorValue = Color.FromArgb(177, 255, 191);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		MaxValue = 3000;
		Value = 0;
		Text = MyPlane.StarPoint.ToString("N0");
		Text = Text.PadLeft(11, ' ');
	}
}
