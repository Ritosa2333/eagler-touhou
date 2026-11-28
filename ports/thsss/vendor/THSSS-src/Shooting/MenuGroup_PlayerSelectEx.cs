using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class MenuGroup_PlayerSelectEx : MenuGroup_PlayerSelect
{
	public MenuGroup_PlayerSelectEx(StageDataPackage StageData)
		: base(StageData, StageSelect: false)
	{
		MenuItemList = new List<BaseMenuItem>();
		PlayerDescription = new List<BaseMenuItem>();
		if (StageData.PData.C_History.FindAll((ClearHistory x) => x.NoContinueClearTimes > 0 && x.MyPlaneFullName == "ReimuA").Count > 0)
		{
			MenuItemList.Add(new CharacterMenuItem(StageData, "FaceReimu_me")
			{
				DestPoint1 = new PointF(-128f, 10f),
				DestPoint2 = new PointF(-228f, 10f),
				TransparentValueF = 0f
			});
			PlayerDescription.Add(new CharacterMenuItem(StageData, "DescriptionReimu")
			{
				DestPoint1 = new PointF(190f, 10f),
				DestPoint2 = new PointF(260f, 10f),
				TransparentValueF = 0f
			});
		}
		StageData.PData.C_History.FindAll((ClearHistory x) => x.ClearTimes > 0);
		if (StageData.PData.C_History.FindAll((ClearHistory x) => x.NoContinueClearTimes > 0 && x.MyPlaneFullName == "MarisaA").Count > 0)
		{
			MenuItemList.Add(new CharacterMenuItem(StageData, "FaceMarisa_me")
			{
				DestPoint1 = new PointF(-128f, 10f),
				DestPoint2 = new PointF(-228f, 10f),
				TransparentValueF = 0f
			});
			PlayerDescription.Add(new CharacterMenuItem(StageData, "DescriptionMarisa")
			{
				DestPoint1 = new PointF(190f, 10f),
				DestPoint2 = new PointF(260f, 10f),
				TransparentValueF = 0f
			});
		}
		StageData.PData.C_History.FindAll((ClearHistory x) => x.ClearTimes > 0);
		if (StageData.PData.C_History.FindAll((ClearHistory x) => x.NoContinueClearTimes > 0 && x.MyPlaneFullName == "SanaeA").Count > 0)
		{
			MenuItemList.Add(new CharacterMenuItem(StageData, "FaceSanae_me")
			{
				DestPoint1 = new PointF(0f, 30f),
				DestPoint2 = new PointF(-100f, 30f),
				TransparentValueF = 0f
			});
			PlayerDescription.Add(new CharacterMenuItem(StageData, "DescriptionSanae")
			{
				DestPoint1 = new PointF(190f, 10f),
				DestPoint2 = new PointF(260f, 10f),
				TransparentValueF = 0f
			});
		}
		StageData.PData.C_History.FindAll((ClearHistory x) => x.ClearTimes > 0);
		if (StageData.PData.C_History.FindAll((ClearHistory x) => x.NoContinueClearTimes > 0 && x.MyPlaneFullName == "KoishiA").Count > 0)
		{
			MenuItemList.Add(new CharacterMenuItem(StageData, "FaceKoishi_me")
			{
				DestPoint1 = new PointF(-128f, 10f),
				DestPoint2 = new PointF(-228f, 10f),
				TransparentValueF = 0f
			});
			PlayerDescription.Add(new CharacterMenuItem(StageData, "DescriptionKoishi")
			{
				DestPoint1 = new PointF(190f, 10f),
				DestPoint2 = new PointF(260f, 10f),
				TransparentValueF = 0f
			});
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		PlayerDescription[MenuSelectIndex].Selected = true;
	}

	public override void ProcessZ()
	{
		Point point = new Point(192, 398);
		switch (MenuItemList[MenuSelectIndex].Name)
		{
		case "Plane":
			StageData.StateSwitchData.SDPswitch.MyPlane = new BaseMyPlane(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "FaceReimu_me":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_Reimu(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "FaceSanae_me":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_Sanae(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "FaceMarisa_me":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_Marisa(StageData.StateSwitchData.SDPswitch, point);
			break;
		case "FaceKoishi_me":
			StageData.StateSwitchData.SDPswitch.MyPlane = new MyPlane_Koishi(StageData.StateSwitchData.SDPswitch, point);
			break;
		default:
			StageData.StateSwitchData.SDPswitch.MyPlane = new BaseMyPlane(StageData.StateSwitchData.SDPswitch, point);
			break;
		}
		StageData.Rep.CreatRpy();
		StageData.StateSwitchData.NextState = "StEx";
		StageData.StateSwitchData.SDPswitch.SetReplayInfo(StageData.StateSwitchData.NextState);
	}

	public override void Show()
	{
		base.Show();
		if (StageData.MenuGroupList[StageData.MenuGroupList.Count - 1] == this)
		{
			PlayerDescription.ForEach((BaseMenuItem x) =>
			{
				x.Show();
			});
		}
	}
}
