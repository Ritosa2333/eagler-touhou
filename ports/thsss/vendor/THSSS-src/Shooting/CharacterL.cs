using System.Drawing;

namespace Shooting;

internal class CharacterL : CharacterR
{
	public CharacterL(StageDataPackage StageData, string textureName)
		: base(StageData, textureName)
	{
		Position = new PointF(BoundRect.Left + 100, BoundRect.Top + 200);
	}

	public override void Move()
	{
		if (Active)
		{
			if (OriginalPosition.X < 150f)
			{
				OriginalPosition = new PointF(OriginalPosition.X + 2f, OriginalPosition.Y);
			}
		}
		else if (OriginalPosition.X > 100f)
		{
			OriginalPosition = new PointF(OriginalPosition.X - 2f, OriginalPosition.Y);
		}
	}
}
