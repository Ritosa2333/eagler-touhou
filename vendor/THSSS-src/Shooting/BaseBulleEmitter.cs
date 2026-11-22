using System.Drawing;

namespace Shooting;

public class BaseBulleEmitter : BaseEnemyPlane
{
	public BaseBulleEmitter(StageDataPackage StageData)
	{
		base.StageData = StageData;
		OriginalPosition = new PointF(0f, 0f);
		Velocity = 0f;
		Direction = 0.0;
		HealthPoint = 1000000f;
		HitEnabled = false;
		EnemyPlaneList.Add(this);
	}

	public override bool HitCheck(BaseObject MyPlane)
	{
		return false;
	}
}
