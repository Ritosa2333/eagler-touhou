using System;
using System.Drawing;

namespace Shooting;

internal class FullPic : BaseObject
{
	public FullPic(StageDataPackage StageData, string textureName)
	{
		base.StageData = StageData;
		Position = new PointF(BoundRect.Left + 250, BoundRect.Top + 150);
		Velocity = 2f;
		Direction = Math.PI / 2.0;
		Active = false;
		LifeTime = 200;
		Accelerate = -0.025f;
		TxtureObject = TextureObjectDictionary[textureName];
		Background2.BackgroundList.Add(this);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Velocity < 0f)
		{
			Velocity = 0f;
		}
		if (Time > 40)
		{
			TransparentVelocity = -5f;
		}
	}
}
