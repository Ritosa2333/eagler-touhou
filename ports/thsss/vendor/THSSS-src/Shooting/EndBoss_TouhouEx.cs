using System.Drawing;
using Shooting.Planes.Story;

namespace Shooting;

internal class EndBoss_TouhouEx : EndBoss_Touhou
{
	public EndBoss_TouhouEx(StageDataPackage StageData, PointF OriginalPosition, Color Color1, Color Color2)
		: base(StageData, OriginalPosition, Color1, Color2)
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == LifeTime)
		{
			new Story_SSSEx_02(StageData);
		}
	}
}
