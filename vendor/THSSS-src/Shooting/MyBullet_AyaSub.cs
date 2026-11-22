using System.Drawing;

namespace Shooting;

public class MyBullet_AyaSub : BaseMyBullet_Touhou
{
	public MyBullet_AyaSub(StageDataPackage StageData, PointF p)
		: base(StageData, "AyaSubBullet00", p, 2f, -1.5707963705062866)
	{
		Damage = 25;
		Region = 21;
		Accelerate = 2f;
		Region = 25;
	}

	public override void GiveEndEffect()
	{
		new Emitter2(StageData, Position);
		new ParticleEndAyaBullet(StageData, "AyaSubBullet", OriginalPosition, 3f, Direction);
	}
}
