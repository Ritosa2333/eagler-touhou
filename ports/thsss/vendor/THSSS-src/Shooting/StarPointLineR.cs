using System.Drawing;

namespace Shooting;

public class StarPointLineR : StarPointLineG
{
	public StarPointLineR(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		type = "B";
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (MyPlane.LastColor == EnchantmentType.Red && Story == null)
		{
			DestPoint = new PointF(66f, 404f);
		}
		else
		{
			DestPoint = new PointF(-6f, 404f);
		}
		Velocity = 3f;
	}
}
