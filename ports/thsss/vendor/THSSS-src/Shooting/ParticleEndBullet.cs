using System;
using System.Drawing;

namespace Shooting;

public class ParticleEndBullet : BaseEffect
{
	public TextureObject[] TxtureObjects { get; set; }

	public int IndexX { get; set; }

	public ParticleEndBullet(StageDataPackage StageData, PointF OriginalPosition, byte ColorType, int Size)
		: this(StageData, OriginalPosition)
	{
		Scale = (float)Size / 32f;
		switch (ColorType)
		{
		case 0:
			ColorValue = Color.White;
			break;
		case 1:
			ColorValue = Color.Red;
			break;
		case 2:
			ColorValue = Color.Red;
			break;
		case 3:
			ColorValue = Color.Violet;
			break;
		case 4:
			ColorValue = Color.Violet;
			break;
		case 5:
			ColorValue = Color.Blue;
			break;
		case 6:
			ColorValue = Color.Blue;
			break;
		case 7:
			ColorValue = Color.Cyan;
			break;
		case 8:
			ColorValue = Color.Cyan;
			break;
		case 9:
			ColorValue = Color.Green;
			break;
		case 10:
			ColorValue = Color.Green;
			break;
		case 11:
			ColorValue = Color.YellowGreen;
			break;
		case 12:
			ColorValue = Color.Yellow;
			break;
		case 13:
			ColorValue = Color.Yellow;
			break;
		case 14:
			ColorValue = Color.Orange;
			break;
		case 15:
			ColorValue = Color.White;
			break;
		default:
			ColorValue = Color.White;
			break;
		}
	}

	public ParticleEndBullet(StageDataPackage StageData, PointF OriginalPosition, Color ColorValue, int Size)
		: this(StageData, OriginalPosition)
	{
		base.ColorValue = ColorValue;
		Scale = (float)Size / 32f;
	}

	public ParticleEndBullet(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		base.OriginalPosition = OriginalPosition;
		Direction = (double)((float)Ran.Next(360) / 180f) * Math.PI;
		TxtureObjects = new TextureObject[8]
		{
			TextureObjectDictionary["EndBullet00"],
			TextureObjectDictionary["EndBullet01"],
			TextureObjectDictionary["EndBullet02"],
			TextureObjectDictionary["EndBullet03"],
			TextureObjectDictionary["EndBullet04"],
			TextureObjectDictionary["EndBullet05"],
			TextureObjectDictionary["EndBullet06"],
			TextureObjectDictionary["EndBullet07"]
		};
		TxtureObject = TxtureObjects[0];
		LifeTime = 31;
		AngularVelocityDegree = Ran.Next(-3, 3);
		ScaleVelocity = 0.05f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		IndexX = Time % 32 / 4;
		TxtureObject = TxtureObjects[IndexX];
		if (Time == LifeTime / 2)
		{
			ScaleVelocity = 0f - ScaleVelocity;
		}
	}
}
