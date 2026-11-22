using System.Drawing;

namespace Shooting;

internal class ParticleWater : BaseEffect
{
	public TextureObject[] TxtureObjects { get; set; }

	public ParticleWater(StageDataPackage StageData, PointF p, float v, double drctn)
		: base(StageData, null, p, v, drctn)
	{
		TxtureObjects = new TextureObject[6]
		{
			TextureObjectDictionary["588-1"],
			TextureObjectDictionary["589-1"],
			TextureObjectDictionary["590-1"],
			TextureObjectDictionary["591-1"],
			TextureObjectDictionary["592-1"],
			TextureObjectDictionary["593-1"]
		};
		LifeTime = 17;
		Ay = 0.6f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		TxtureObject = TxtureObjects[Time % 18 / 3];
	}
}
