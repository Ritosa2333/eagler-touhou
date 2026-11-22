using System.Drawing;

namespace Shooting;

public class ParticleEndAyaBullet : BaseEffect
{
	public TextureObject[] TxtureObjects { get; set; }

	public int IndexX { get; set; }

	public ParticleEndAyaBullet(StageDataPackage StageData, string Name, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, null, OriginalPosition, Velocity, Direction)
	{
		base.OriginalPosition = OriginalPosition;
		TxtureObjects = new TextureObject[4]
		{
			TextureObjectDictionary[Name + "00"],
			TextureObjectDictionary[Name + "01"],
			TextureObjectDictionary[Name + "02"],
			TextureObjectDictionary[Name + "03"]
		};
		TxtureObject = TxtureObjects[0];
		LifeTime = 15;
		Angle = 1.5707963705062866;
		MaxTransparent = 80;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		IndexX = Time % 16 / 4;
		TxtureObject = TxtureObjects[IndexX];
		TransparentValueF -= MaxTransparent / LifeTime;
		Scale += 0.1f;
	}
}
