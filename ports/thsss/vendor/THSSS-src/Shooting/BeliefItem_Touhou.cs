using System.Drawing;

namespace Shooting;

internal class BeliefItem_Touhou : BaseItem_Touhou
{
	public BeliefItem_Touhou(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, "Item-2", OriginalPosition)
	{
		Region = 5;
	}

	public override void HitCheckAll()
	{
		base.HitCheckAll();
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			MyPlane.HighItemScore += 300;
			MyPlane.StarPoint += 20;
			StageData.SoundPlay("se_item00.wav", OriginalPosition.X / (float)BoundRect.Width);
		}
	}

	public override void Show()
	{
		base.Show();
		if (OriginalPosition.Y < -8f)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["Item-10"], 1f, 0f, new PointF(Position.X, BoundRect.Top), Color.FromArgb(TransparentValue, ColorValue));
		}
	}
}
