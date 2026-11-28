using System;
using System.Drawing;

namespace Shooting;

public class Background2DRemover : BaseEffect
{
	public Background2DRemover(StageDataPackage StageData)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		LifeTime = 20;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Background.BackgroundList.ForEach((BaseObject x) =>
		{
			x.TransparentValueF -= MaxTransparent / LifeTime;
		});
		if (Time >= LifeTime)
		{
			Background.BackgroundList.Clear();
		}
	}

	public override void Show()
	{
	}
}
