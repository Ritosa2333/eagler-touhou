using System;
using System.Drawing;

namespace Shooting;

internal class GameTitle : BaseEffect
{
	public GameTitle(StageDataPackage StageData, string textureName, Point Position, Point DestPoint)
		: base(StageData)
	{
		TxtureObject = TextureObjectDictionary[textureName];
		base.Position = Position;
		base.DestPoint = DestPoint;
		Velocity = 5f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Angle = 0f - (float)(Direction - Math.PI / 2.0);
		TransparentValueF += 2f;
	}
}
