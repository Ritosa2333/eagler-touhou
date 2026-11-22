using System.Drawing;

namespace Shooting;

public class Item_Line_Red : Item_Line
{
	public Item_Line_Red(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "RedNum_";
	}

	public override void Ctrl()
	{
		base.Ctrl();
		MaxValue = 400;
		Value = 0;
		Text = ((float)MyPlane.Power / 100f).ToString("f2") + "/4.00";
		Text = Text.PadLeft(11, ' ');
	}
}
