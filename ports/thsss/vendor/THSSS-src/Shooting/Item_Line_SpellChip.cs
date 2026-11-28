using System.Drawing;

namespace Shooting;

public class Item_Line_SpellChip : Item_Line
{
	public Item_Line_SpellChip(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		FontType = "ShardNum_";
		Delta = 9;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Value = 0;
		string text = MyPlane.SpellChip.ToString();
		int nextSpellChip = MyPlane.nextSpellChip;
		Text = text + "/" + nextSpellChip;
		Text = Text.PadLeft(5, ' ');
	}
}
