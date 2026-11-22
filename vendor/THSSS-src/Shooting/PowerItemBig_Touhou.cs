using System.Drawing;

namespace Shooting;

internal class PowerItemBig_Touhou : BaseItem_Touhou
{
	public PowerItemBig_Touhou(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, "Item-3", OriginalPosition)
	{
		Region = 10;
	}

	public override void HitCheckAll()
	{
		base.HitCheckAll();
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			float num = MyPlane.Power;
			MyPlane.Power += 100;
			StageData.SoundPlay("se_item00.wav", OriginalPosition.X / (float)BoundRect.Width);
			if (num != (float)MyPlane.Power)
			{
				StageData.SoundPlay("se_powerup.wav");
			}
			int num2 = 5000 + MyPlane.GreenPoint / 2;
			num2 /= 10;
			num2 *= 10;
			MyPlane.Score += num2;
			ParticleScoreValue particleScoreValue = new ParticleScoreValue(StageData, OriginalPosition, num2);
			if (Doubled)
			{
				particleScoreValue.ColorValue = Color.FromArgb(254, 222, 63);
			}
		}
	}

	public override void Show()
	{
		base.Show();
		if (OriginalPosition.Y < -8f)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["Item-11"], 1f, 0f, new PointF(Position.X, BoundRect.Top), Color.FromArgb(TransparentValue, ColorValue));
		}
	}
}
