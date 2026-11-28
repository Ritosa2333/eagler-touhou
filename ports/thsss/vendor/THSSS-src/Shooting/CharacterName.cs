using System;
using System.Drawing;

namespace Shooting;

internal class CharacterName : BaseStoryItem
{
	public CharacterName(StageDataPackage StageData, string textureName)
		: base(StageData)
	{
		Position = new PointF(BoundRect.Right - 70, BoundRect.Top + 300);
		Velocity = 0f;
		Direction = Math.PI / 2.0;
		TransparentValueF = 0f;
		TxtureObject = TextureObjectDictionary[textureName];
		TransparentVelocityDictionary.Add(10, 20f);
		TransparentVelocityDictionary.Add(210, -10f);
	}
}
