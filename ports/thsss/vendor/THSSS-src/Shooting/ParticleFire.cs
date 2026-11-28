using System.Drawing;

namespace Shooting;

internal class ParticleFire : BaseEffect
{
	public int ColorType { get; set; }

	public ParticleFire(StageDataPackage StageData, PointF Position, float Velocity, double Direcity)
		: base(StageData, null, Position, Velocity, Direcity)
	{
		LifeTime = 31;
		Time = Ran.Next(32);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		switch (ColorType)
		{
		case 0:
			TxtureObject = TextureObjectDictionary["FY_Particle0_" + Time % 32 / 4];
			break;
		case 1:
			TxtureObject = TextureObjectDictionary["FY_Particle1_" + Time % 32 / 4];
			break;
		case 2:
			TxtureObject = TextureObjectDictionary["FY_Particle2_" + Time % 32 / 4];
			break;
		}
	}
}
