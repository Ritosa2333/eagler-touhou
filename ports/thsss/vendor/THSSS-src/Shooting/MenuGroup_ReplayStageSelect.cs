using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;

namespace Shooting;

internal class MenuGroup_ReplayStageSelect : BaseMenuGroup
{
	private string RepIndex;

	private const string NullDescription = "----------";

	public MenuGroup_ReplayStageSelect(StageDataPackage StageData, PointF OriginalPosition, DescriptionMenuItem RepMenuItem)
		: base(StageData)
	{
		RepIndex = RepMenuItem.Name;
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>();
		for (int i = 0; i < 6; i++)
		{
			MenuItemList.Add(new DescriptionMenuItem(StageData, $"Stage{i + 1:00}")
			{
				Description = "----------"
			});
		}
		MenuItemList.Add(new DescriptionMenuItem(StageData, "StageEx")
		{
			Description = "----------"
		});
		float x = OriginalPosition.X + 12f;
		float num = OriginalPosition.Y + 12f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.Position = new PointF(x, OriginalPosition.Y + 10f);
			menuItem.DestPoint = new PointF(x, num);
			num += 17f;
		}
		string text = ".\\Replay\\thSSS_" + RepIndex + ".rpy";
		if (File.Exists(text))
		{
			ReplayInfo replayInfo = Replay.ReadTitle(text);
			if (replayInfo.StartStage == "StEx")
			{
				((DescriptionMenuItem)MenuItemList[6]).Description = replayInfo.MyPlaneData[1].Score.ToString().PadLeft(10);
			}
			else
			{
				int num2 = ((!replayInfo.StartStage.Contains("St")) ? Convert.ToInt32(replayInfo.StartStage.Replace("Bs", "")) : Convert.ToInt32(replayInfo.StartStage.Replace("St", "")));
				for (int j = 0; j < replayInfo.MyPlaneData.Count - 1; j++)
				{
					if (num2 - 1 + j < 6)
					{
						((DescriptionMenuItem)MenuItemList[num2 - 1 + j]).Description = replayInfo.MyPlaneData[j + 1].Score.ToString().PadLeft(10);
					}
				}
			}
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTilte = new DescriptionMenuItem(StageData, RepIndex)
		{
			Description = RepMenuItem.Description,
			OriginalPosition = RepMenuItem.OriginalPosition,
			Selected = true
		};
		MenuTitlePos1 = new PointF(RepMenuItem.OriginalPosition.X, 16f);
		MenuTitlePos2 = RepMenuItem.OriginalPosition;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		base.OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
		ColorValue = Color.SkyBlue;
	}

	public override void ProcessKeys()
	{
		base.ProcessKeys();
		if (KClass.Key_Z && LastZ == 0)
		{
			MenuItemList[MenuSelectIndex].Click();
			OnChangeMenu = TimeMain + 1;
		}
		if ((!KClass.Key_X && !KClass.Key_ESC) || LastX != 0)
		{
			return;
		}
		OnRemoveMenu = TimeMain + 20;
		TransparentVelocity = -15f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(MenuItemList[0].OriginalPosition.X, MenuItemList[0].OriginalPosition.Y);
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void ProcessZ()
	{
		if (((DescriptionMenuItem)MenuItemList[MenuSelectIndex]).Description != "----------")
		{
			StageData.SoundPlay("se_ok00.wav");
			string fileName = ".\\Replay\\thSSS_" + RepIndex + ".rpy";
			StageData.Rep.LoadRpy(fileName);
			ReplayInfo replayInfo = Replay.ReadTitle(fileName);
			StageData.RepInfo = replayInfo;
			string text;
			if (MenuSelectIndex >= 6)
			{
				text = "StEx";
			}
			else
			{
				text = ((!replayInfo.StartStage.Contains("St")) ? ("Bs" + (MenuSelectIndex + 1)) : ("St" + (MenuSelectIndex + 1)));
			}
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = text,
				NeedInit = true,
				SDPswitch = new StageDataPackage(StageData.GlobalData)
				{
					OnReplay = true,
					Difficulty = replayInfo.Rank
				}
			};
			int num = ((!(text == "StEx")) ? ((!replayInfo.StartStage.Contains("St")) ? (MenuSelectIndex - Convert.ToInt32(replayInfo.StartStage.Replace("Bs", "")) + 1) : (MenuSelectIndex - Convert.ToInt32(replayInfo.StartStage.Replace("St", "")) + 1)) : 0);
			Point point = new Point(192, 398);
			if (num >= 0)
			{
				point = new Point((int)replayInfo.MyPlaneData[num].PosX, (int)replayInfo.MyPlaneData[num].PosY);
			}
			BaseMyPlane baseMyPlane;
			switch (replayInfo.MyPlaneName)
			{
			case "Aya":
			{
				string weaponType = replayInfo.WeaponType;
				if (weaponType == "A")
				{
					baseMyPlane = new MyPlane_Aya(StageData.StateSwitchData.SDPswitch, point);
				}
				else
				{
					baseMyPlane = ((!(weaponType == "B")) ? new BaseMyPlane(StageData.StateSwitchData.SDPswitch, point) : new MyPlane_AyaB(StageData.StateSwitchData.SDPswitch, point));
				}
				break;
			}
			case "Plane":
			{
				string weaponType = replayInfo.WeaponType;
				if (weaponType == "A")
				{
					baseMyPlane = new BaseMyPlane(StageData.StateSwitchData.SDPswitch, point);
				}
				else
				{
					baseMyPlane = ((!(weaponType == "B")) ? new BaseMyPlane(StageData.StateSwitchData.SDPswitch, point) : new MyPlane_PlaneB(StageData.StateSwitchData.SDPswitch, point));
				}
				break;
			}
			case "Reimu":
				baseMyPlane = new MyPlane_Reimu(StageData.StateSwitchData.SDPswitch, point);
				break;
			case "Sanae":
				baseMyPlane = new MyPlane_Sanae(StageData.StateSwitchData.SDPswitch, point);
				break;
			case "Marisa":
				baseMyPlane = new MyPlane_Marisa(StageData.StateSwitchData.SDPswitch, point);
				break;
			case "Koishi":
				baseMyPlane = new MyPlane_Koishi(StageData.StateSwitchData.SDPswitch, point);
				break;
			default:
				baseMyPlane = new BaseMyPlane(StageData.StateSwitchData.SDPswitch, point);
				break;
			}
			if (num >= 0)
			{
				baseMyPlane.Life = replayInfo.MyPlaneData[num].Life;
				baseMyPlane.Spell = replayInfo.MyPlaneData[num].Spell;
				baseMyPlane.Power = replayInfo.MyPlaneData[num].Power;
				baseMyPlane.Score = replayInfo.MyPlaneData[num].Score;
				baseMyPlane.OriginalPosition = new PointF(replayInfo.MyPlaneData[num].PosX, replayInfo.MyPlaneData[num].PosY);
				baseMyPlane.Graze = replayInfo.MyPlaneData[num].Graze;
				baseMyPlane.LifeUpCount = replayInfo.MyPlaneData[num].LifeUpCount;
				baseMyPlane.LifeChip = replayInfo.MyPlaneData[num].LifeChip;
				baseMyPlane.SpellChip = replayInfo.MyPlaneData[num].SpellChip;
				baseMyPlane.StarPoint = replayInfo.MyPlaneData[num].StarPoint;
				baseMyPlane.HighItemScore = replayInfo.MyPlaneData[num].HighItemScore;
				baseMyPlane.Rate = replayInfo.MyPlaneData[num].Rate;
				baseMyPlane.LastColor = replayInfo.MyPlaneData[num].LastColor;
				StageData.Rep.DataPosition = replayInfo.MyPlaneData[num].DataPosition;
			}
			StageData.StateSwitchData.SDPswitch.MyPlane = baseMyPlane;
			StageData.StateSwitchData.SDPswitch.RepInfo = replayInfo;
		}
	}
}
