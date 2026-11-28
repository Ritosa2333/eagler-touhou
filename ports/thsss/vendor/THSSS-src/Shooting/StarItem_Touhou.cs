using System.Drawing;

namespace Shooting;

internal class StarItem_Touhou : BaseItem_Touhou
{
	public StarItem_Touhou(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, "SmallStar", OriginalPosition)
	{
		Region = 5;
		AngularVelocity = 0f;
		DirectionDegree += (float)Ran.Next(-49, 50) / 10f;
		MaxTransparent = 80;
		TransparentValueF = 0f;
		TransparentVelocity = 16f;
		Active = true;
		AngleWithDirection = false;
	}

	public override void HitCheckAll()
	{
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			MyPlane.StarPoint++;
			MyPlane.Score += 100L;
			if (++MyPlane.GreenPoint % 2 == 0)
			{
				MyPlane.HighItemScore += 10;
			}
			StageData.SoundPlay("se_item00.wav", OriginalPosition.X / (float)BoundRect.Width);
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 40)
		{
			Obtain = true;
		}
		Angle = 0.0;
	}
}
