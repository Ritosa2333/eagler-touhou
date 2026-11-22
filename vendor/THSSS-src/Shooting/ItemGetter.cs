using System;
using System.Drawing;

namespace Shooting;

public class ItemGetter : BaseEffect
{
	public ItemGetter(StageDataPackage StageData)
		: base(StageData, null, new Point(0, 0), 0f, Math.PI / 2.0)
	{
		LifeTime = 2;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		ItemList.ForEach((BaseItem x) =>
		{
			x.Obtain = true;
			x.Doubled = true;
		});
	}

	public override void Show()
	{
	}
}
