using System.Drawing;

namespace Shooting;

public class StarPointLineB : StarPointLineG
{
	public StarPointLineB(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, OriginalPosition)
	{
		type = "A";
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (MyPlane.LastColor == EnchantmentType.Blue && Story == null)
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
