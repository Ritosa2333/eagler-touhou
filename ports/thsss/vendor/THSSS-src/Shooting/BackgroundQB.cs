using System;
using System.Drawing;

namespace Shooting;

internal class BackgroundQB : BaseObject
{
	public BackgroundQB(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, Math.PI / 2.0)
	{
		Background.BackgroundList.Add(this);
		OriginalPosition = new PointF(BoundRect.Width / 2, 200f);
		TransparentValueF = 0f;
		Scale = 0.9f;
	}

	public override void Ctrl()
	{
		Time++;
		if (TransparentValueF < 200f)
		{
			TransparentValueF += 5f;
		}
	}
}
