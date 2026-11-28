using System.Drawing;

namespace Shooting;

public class Item_Line_LifeChip : Item_Line
{
	public Item_Line_LifeChip(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "ShardNum_";
		Delta = 9;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Value = 0;
		Text = MyPlane.LifeChip + "/" + MyPlane.nextLifeChip[MyPlane.LifeUpCount];
		Text = Text.PadLeft(5, ' ');
	}
}
