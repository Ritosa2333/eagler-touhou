using System.Drawing;

namespace Shooting;

internal class Bullet_TouhouFire : Bullet_Touhou_01
{
	private int timeOffset;

	public TextureObject[] TxtureObjects { get; set; }

	public Bullet_TouhouFire(StageDataPackage StageData, int type, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, null, OriginalPosition, Velocity, Direction, 0)
	{
		switch (type)
		{
		case 0:
			ColorType = 1;
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet30-0"],
				TextureObjectDictionary["bullet30-1"],
				TextureObjectDictionary["bullet30-2"],
				TextureObjectDictionary["bullet30-3"]
			};
			Active = false;
			break;
		case 1:
			ColorType = 1;
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet17-0"],
				TextureObjectDictionary["bullet17-1"],
				TextureObjectDictionary["bullet17-2"],
				TextureObjectDictionary["bullet17-3"]
			};
			Angle = 3.1415927410125732;
			Active = true;
			break;
		case 2:
			ColorType = 7;
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet18-0"],
				TextureObjectDictionary["bullet18-1"],
				TextureObjectDictionary["bullet18-2"],
				TextureObjectDictionary["bullet18-3"]
			};
			Angle = 3.1415927410125732;
			Active = true;
			break;
		case 3:
			ColorType = 4;
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet42_0"],
				TextureObjectDictionary["bullet42_1"],
				TextureObjectDictionary["bullet42_2"],
				TextureObjectDictionary["bullet42_3"]
			};
			Angle = 3.1415927410125732;
			Active = true;
			break;
		case 4:
			ColorType = 14;
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet43_0"],
				TextureObjectDictionary["bullet43_1"],
				TextureObjectDictionary["bullet43_2"],
				TextureObjectDictionary["bullet43_3"]
			};
			Angle = 3.1415927410125732;
			Active = true;
			break;
		case 5:
			ColorType = 14;
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet44_0"],
				TextureObjectDictionary["bullet44_1"],
				TextureObjectDictionary["bullet44_2"],
				TextureObjectDictionary["bullet44_3"]
			};
			Angle = 3.1415927410125732;
			Active = true;
			break;
		default:
			TxtureObjects = new TextureObject[4]
			{
				TextureObjectDictionary["bullet30-0"],
				TextureObjectDictionary["bullet30-1"],
				TextureObjectDictionary["bullet30-2"],
				TextureObjectDictionary["bullet30-3"]
			};
			Active = true;
			break;
		}
		timeOffset = Ran.Next(16);
		TxtureObject = TxtureObjects[(Time + timeOffset) % 16 / 4];
	}

	public override void Ctrl()
	{
		base.Ctrl();
		TxtureObject = TxtureObjects[(Time + timeOffset) % 16 / 4];
	}
}
