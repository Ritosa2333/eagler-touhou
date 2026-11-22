using System.Drawing;

namespace Shooting;

internal class CharacterL_Touhou : CharacterR_Touhou
{
	public CharacterL_Touhou(StageDataPackage StageData, string textureName)
		: base(StageData, textureName)
	{
		OriginalPosition = new PointF(70f, 128f);
	}

	public override void Move()
	{
		if (Active)
		{
			if (OriginalPosition.X < 100f)
			{
				OriginalPosition = new PointF(OriginalPosition.X + 2f, OriginalPosition.Y);
			}
		}
		else if (OriginalPosition.X > 70f)
		{
			OriginalPosition = new PointF(OriginalPosition.X - 2f, OriginalPosition.Y);
		}
	}
}
