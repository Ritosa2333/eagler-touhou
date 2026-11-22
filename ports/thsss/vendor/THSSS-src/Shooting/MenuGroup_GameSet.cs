using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_GameSet : MenuGroup_GameOver
{
	public MenuGroup_GameSet(StageDataPackage StageData)
		: base(StageData)
	{
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>();
		if (!StageData.GlobalData.LastState.StageData.OnReplay)
		{
			if (StageData.GlobalData.LastState.StageData.ContinueTimes == 0)
			{
				MenuItemList.Add(new BaseMenuItem(StageData, "Menu_保存录像"));
			}
		}
		else
		{
			StageData.OnReplay = false;
		}
		MenuItemList.Add(new BaseMenuItem(StageData, "Menu_从头开始"));
		MenuItemList.Add(new BaseMenuItem(StageData, "Menu_返回主菜单"));
		int num = 62;
		int num2 = 300;
		int num3 = -200;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(num, num2);
			menuItem.Position = new PointF(num + num3, 150f);
			num2 += 30;
			num3 -= 50;
		}
		MenuItemList[MenuSelectIndex].Selected = true;
	}
}
