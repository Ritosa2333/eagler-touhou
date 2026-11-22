using System.Drawing;
using Shooting.Planes.Story;

namespace Shooting;

internal class EndBoss_Touhou03 : EndBoss_Touhou
{
	public EndBoss_Touhou03(StageDataPackage StageData, PointF OriginalPosition, Color Color1, Color Color2)
		: base(StageData, OriginalPosition, Color1, Color2)
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == LifeTime)
		{
			switch (MyPlane.Name)
			{
			case "Aya":
				new Story_SSS03_02(StageData);
				break;
			case "Reimu":
				new Story_SSS03_02A(StageData);
				break;
			case "Marisa":
				new Story_SSS03_02B(StageData);
				break;
			case "Sanae":
				new Story_SSS03_02C(StageData);
				break;
			case "Koishi":
				new Story_SSS03_02D(StageData);
				break;
			default:
				new Story_SSS03_02X(StageData);
				break;
			}
		}
	}
}
