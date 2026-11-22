using System.Drawing;

namespace Shooting;

internal class PowerItem_Touhou : BaseItem_Touhou
{
	public PowerItem_Touhou(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, "Item-0", OriginalPosition)
	{
		Region = 5;
	}

	public override void HitCheckAll()
	{
		base.HitCheckAll();
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			float num = MyPlane.Power;
			MyPlane.Power += 2;
			if (num != (float)MyPlane.Power && MyPlane.Power % 100 == 0)
			{
				StageData.SoundPlay("se_powerup.wav");
			}
			int num2 = ((MyPlane.Power == 400) ? 51200 : 10);
			MyPlane.Score += num2;
			MyPlane.StarPoint += 20;
			if (MyPlane.Power == 400)
			{
				new ParticleScoreValue(StageData, OriginalPosition, num2);
			}
			StageData.SoundPlay("se_item00.wav", OriginalPosition.X / (float)BoundRect.Width);
		}
	}

	public override void Show()
	{
		base.Show();
		if (OriginalPosition.Y < -8f)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["Item-8"], 1f, 0f, new PointF(Position.X, BoundRect.Top), Color.FromArgb(TransparentValue, ColorValue));
		}
	}
}
