using System.Drawing;

namespace Shooting;

internal class ScoreItem_Touhou2 : BaseItem_Touhou
{
	public ScoreItem_Touhou2(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, "Item-7", OriginalPosition)
	{
		Region = 5;
		AngularVelocity = 0f;
		DirectionDegree += (float)Ran.Next(-49, 50) / 10f;
		MaxTransparent = 80;
		TransparentValueF = 0f;
		TransparentVelocity = 16f;
		Active = true;
	}

	public override void HitCheckAll()
	{
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
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
	}
}
