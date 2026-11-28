using System.Drawing;

namespace Shooting;

internal class FullPic2 : BaseObject
{
	public FullPic2(StageDataPackage StageData, string textureName)
	{
		base.StageData = StageData;
		Position = new PointF(BoundRect.Left + 440, BoundRect.Top + 200);
		Velocity = 9f;
		DirectionDegree = 165.0;
		Active = false;
		LifeTime = 200;
		Accelerate = -0.2f;
		AccelerateDictionary.Add(90, 1f);
		TransparentVelocityDictionary.Add(120, -6f);
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
