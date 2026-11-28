using System;
using System.Drawing;

namespace Shooting;

public class ParticleBeginBullet : BaseEffect
{
	public TextureObject[] TxtureObjects { get; set; }

	public int IndexX { get; set; }

	public ParticleBeginBullet(StageDataPackage StageData, PointF Position, byte ColorType)
		: this(StageData, Position)
	{
		switch (ColorType)
		{
		case 0:
			TxtureObject = TextureObjectDictionary["bullet25_0"];
			break;
		case 1:
			TxtureObject = TextureObjectDictionary["bullet25_1"];
			break;
		case 2:
			TxtureObject = TextureObjectDictionary["bullet25_1"];
			break;
		case 3:
			TxtureObject = TextureObjectDictionary["bullet25_2"];
			break;
		case 4:
			TxtureObject = TextureObjectDictionary["bullet25_2"];
			break;
		case 5:
			TxtureObject = TextureObjectDictionary["bullet25_3"];
			break;
		case 6:
			TxtureObject = TextureObjectDictionary["bullet25_3"];
			break;
		case 7:
			TxtureObject = TextureObjectDictionary["bullet25_4"];
			break;
		case 8:
			TxtureObject = TextureObjectDictionary["bullet25_4"];
			break;
		case 9:
			TxtureObject = TextureObjectDictionary["bullet25_5"];
			break;
		case 10:
			TxtureObject = TextureObjectDictionary["bullet25_5"];
			break;
		case 11:
			TxtureObject = TextureObjectDictionary["bullet25_5"];
			break;
		case 12:
			TxtureObject = TextureObjectDictionary["bullet25_6"];
			break;
		case 13:
			TxtureObject = TextureObjectDictionary["bullet25_6"];
			break;
		case 14:
			TxtureObject = TextureObjectDictionary["bullet25_6"];
			break;
		case 15:
			TxtureObject = TextureObjectDictionary["bullet25_7"];
			break;
		default:
			TxtureObject = TextureObjectDictionary["bullet25_0"];
			break;
		}
	}

	public ParticleBeginBullet(StageDataPackage StageData, PointF Position)
		: base(StageData, null, Position, 0f, 0.0)
	{
		Direction = (double)Ran.Next(360) * Math.PI / 180.0;
		TxtureObject = TextureObjectDictionary["bullet25_0"];
		LifeTime = 8;
		Scale = 2f;
		Active = false;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (LifeTime - 8 < Time && Time < LifeTime)
		{
			TransparentValueF -= MaxTransparent / 8;
		}
	}
}
