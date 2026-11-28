using System.Drawing;

namespace Shooting;

internal class LifeItem_Touhou : BaseItem_Touhou
{
	public LifeItem_Touhou(StageDataPackage StageData, PointF p)
		: base(StageData, "Item-6", p)
	{
		Region = 10;
	}

	public override void HitCheckAll()
	{
		base.HitCheckAll();
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			MyPlane.Extend();
		}
	}

	public override void Show()
	{
		base.Show();
		if (OriginalPosition.Y < -8f)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["Item-14"], 1f, 0f, new PointF(Position.X, BoundRect.Top), Color.FromArgb(TransparentValue, ColorValue));
		}
	}
}
