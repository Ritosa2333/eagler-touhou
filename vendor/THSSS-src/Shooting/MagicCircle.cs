using System;
using System.Drawing;

namespace Shooting;

internal class MagicCircle : BaseEffect
{
	public MagicCircle(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
	}

	public override void Move()
	{
		if (Boss != null)
		{
			OriginalPosition = Boss.OriginalPosition;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Angle += 0.019999999552965164;
		Scale += (float)Math.Sin((double)((float)Time * 3f / 180f) * Math.PI) / 80f;
		TransparentValueF = 100 + (int)(Math.Sin((double)((float)Time * 2f / 180f) * Math.PI) * 75.0);
	}

	public override bool OutBoundary()
	{
		if (Boss == null)
		{
			return true;
		}
		return false;
	}
}
