using System.Drawing;

namespace Shooting;

internal class SpellItem_Touhou : BaseItem_Touhou
{
	public SpellItem_Touhou(StageDataPackage StageData, PointF p)
		: base(StageData, "SpellItem", p)
	{
		Region = 10;
		Scale = 1f;
	}

	public override void HitCheckAll()
	{
		base.HitCheckAll();
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			MyPlane.SpellExtand();
		}
	}

	public override void Show()
	{
		base.Show();
	}
}
