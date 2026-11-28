using System.Drawing;

namespace Shooting;

public class MyBullet_Aya : BaseMyBullet_Touhou
{
	public MyBullet_Aya(StageDataPackage StageData, PointF p)
		: base(StageData, "AyaBullet00", p, 30f, -1.5707963705062866)
	{
		Damage = 20;
	}

	public override void GiveEndEffect()
	{
		new Emitter2(StageData, Position);
		new ParticleEndAyaBullet(StageData, "AyaBullet", OriginalPosition, 3f, Direction);
	}
}
