using System.Drawing;

namespace Shooting;

public class BackgroundBoss06 : BaseObject
{
	public BackgroundBoss06(StageDataPackage StageData)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		new BackgroundSC(StageData, "底色");
		BackgroundSC bsc1 = new BackgroundSC(StageData, "绿光");
		bsc1.CtrlCall = (string x) =>
		{
			if (bsc1.Time % 300 == 0)
			{
				bsc1.TransparentVelocity = -2.55f;
			}
			else if (bsc1.Time % 300 == 100)
			{
				bsc1.TransparentVelocity = 2.55f;
			}
		};
		BackgroundSC bsc2 = new BackgroundSC(StageData, "蓝光");
		bsc2.CtrlCall = (string x) =>
		{
			if (bsc2.Time % 300 == 100)
			{
				bsc2.TransparentVelocity = -2.55f;
			}
			else if (bsc2.Time % 300 == 200)
			{
				bsc2.TransparentVelocity = 2.55f;
			}
		};
		BackgroundSC bsc3 = new BackgroundSC(StageData, "红光");
		bsc3.CtrlCall = (string x) =>
		{
			if (bsc3.Time % 300 == 200)
			{
				bsc3.TransparentVelocity = -2.55f;
			}
			else if (bsc3.Time % 300 == 0)
			{
				bsc3.TransparentVelocity = 2.55f;
			}
		};
	}
}
