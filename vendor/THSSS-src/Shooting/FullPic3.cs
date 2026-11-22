using System.Drawing;

namespace Shooting;

internal class FullPic3 : BaseObject
{
	public FullPic3(StageDataPackage StageData, string textureName)
	{
		base.StageData = StageData;
		Position = new PointF(BoundRect.Left + BoundRect.Width / 2, BoundRect.Bottom + 100);
		Velocity = 9f;
		DirectionDegree = -90.0;
		Active = false;
		LifeTime = 200;
		Accelerate = -0.2f;
		AccelerateDictionary.Add(90, 0.1f);
		TransparentVelocityDictionary.Add(90, -10f);
		AngleWithDirection = false;
		AngleDegree = 90.0;
		TxtureObject = TextureObjectDictionary[textureName];
		Background2.BackgroundList.Add(this);
		OutsideRegion = 500;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Velocity < 1.2f)
		{
			Velocity = 1.2f;
		}
	}
}
