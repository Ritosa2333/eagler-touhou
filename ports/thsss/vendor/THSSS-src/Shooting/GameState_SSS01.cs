using System;
using System.Drawing;
using System.IO;
using Shooting.Planes.Story;

namespace Shooting;

internal class GameState_SSS01 : BaseStage, IGameState
{
	public GameState_SSS01(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "St1";
	}

	public override void Drama()
	{
		base.Drama();
		int[] array = new int[3] { 3180, 4000, 6440 };
		if (TimeMain == 1)
		{
			switch ((int)StageData.Difficulty)
			{
			case 2:
				MyPlane.EnchantmentCountNeeded = 3;
				break;
			case 3:
				MyPlane.EnchantmentCountNeeded = 4;
				break;
			}
			new TransitionIn(StageData);
		}
		if (TimeMain == 2)
		{
			if (StageData.C_History.PracticeLevel < 1)
			{
				StageData.C_History.PracticeLevel = 1;
			}
			StageData.ChangeBGM(".\\BGM\\Stage01.wav", (testStartTime + 60) * 44100 / 60, 0, 0, 0, 0);
			new MusicTitle(StageData, "宁静夏夜的微风", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Background3D.Envi = new EnvironmentSSS01(StageData);
			new BackgroundTransitionIn(StageData)
			{
				Delay = 200,
				LifeTime = 320
			};
			new BackgroundGroupSSS01(StageData);
			if (testStartTime > TimeMain)
			{
				TimeMain = testStartTime;
			}
			if (TimeMain < array[0])
			{
				string[] array2 = (int)StageData.Difficulty switch
				{
					0 => Directory.GetFiles(".\\CS\\St01\\A\\E\\", "*.mbg"), 
					1 => Directory.GetFiles(".\\CS\\St01\\A\\N\\", "*.mbg"), 
					2 => Directory.GetFiles(".\\CS\\St01\\A\\H\\", "*.mbg"), 
					3 => Directory.GetFiles(".\\CS\\St01\\A\\L\\", "*.mbg"), 
					_ => Directory.GetFiles(".\\CS\\St01\\A\\L\\", "*.mbg"), 
				};
				foreach (string fileName in array2)
				{
					CS_Data cSData = StageData.LoadCS(fileName);
					new CSEmitterController(StageData, cSData)
					{
						OnRoad = true,
						Time = TimeMain + 60
					};
				}
			}
			new TextTwinkle(StageData, StageData.Difficulty.ToString(), new PointF(BoundRect.X + BoundRect.Width / 2, BoundRect.Y + 16), 0f, Math.PI / 2.0)
			{
				TwinkleColor = Color.Gray,
				Circle = 10,
				LifeTime = 150
			};
		}
		else if (TimeMain == 30)
		{
			new TextTwinkle(StageData, "ItemGetBorderLine", new PointF(BoundRect.X + BoundRect.Width / 2, BoundRect.Y + 130), 0f, Math.PI / 2.0).TwinkleColor = Color.OrangeRed;
			new TextTwinkle(StageData, "ItemGetBorderLine01", new PointF(BoundRect.X + BoundRect.Width / 2 - 168, BoundRect.Y + 130), 0f, Math.PI / 2.0).TwinkleColor = Color.OrangeRed;
			new TextTwinkle(StageData, "ItemGetBorderLine01", new PointF(BoundRect.X + BoundRect.Width / 2 + 158, BoundRect.Y + 130), 0f, Math.PI / 2.0).TwinkleColor = Color.OrangeRed;
		}
		if (TimeMain == array[0])
		{
			StageData.RemoveBullets();
			new Boss_Ami01(StageData);
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(40, 255, 20));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
			Background3D.WarpColorKey = 0;
		}
		else if (TimeMain > array[1] && Boss == null && !RoadFlag)
		{
			RoadFlag = true;
			string[] array2 = (int)StageData.Difficulty switch
			{
				0 => Directory.GetFiles(".\\CS\\St01\\B\\E\\", "*.mbg"), 
				1 => Directory.GetFiles(".\\CS\\St01\\B\\N\\", "*.mbg"), 
				2 => Directory.GetFiles(".\\CS\\St01\\B\\H\\", "*.mbg"), 
				3 => Directory.GetFiles(".\\CS\\St01\\B\\L\\", "*.mbg"), 
				_ => Directory.GetFiles(".\\CS\\St01\\B\\L\\", "*.mbg"), 
			};
			for (int i = 0; i < array2.Length; i++)
			{
				CS_Data cS_Data = new CS_Data(array2[i]);
				cS_Data.String2Data(StageData);
				new CSEmitterController(StageData, cS_Data)
				{
					OnRoad = true,
					Time = TimeMain + 60
				};
			}
		}
		else if (array[2] - 130 == TimeMain)
		{
			new BossStar(StageData, new PointF(BoundRect.Width / 2, 150f)).ColorType = 6;
		}
		else if (array[2] == TimeMain)
		{
			StageData.RemoveBullets();
			new Boss_Ami02(StageData);
			switch (MyPlane.Name)
			{
			case "Aya":
				new Story_SSS01_01(StageData);
				break;
			case "Reimu":
				new Story_SSS01_01A(StageData);
				break;
			case "Marisa":
				new Story_SSS01_01B(StageData);
				break;
			case "Sanae":
				new Story_SSS01_01C(StageData);
				break;
			case "Koishi":
				new Story_SSS01_01D(StageData);
				break;
			default:
				new Story_SSS01_01X(StageData);
				break;
			}
			Background3D.WarpEnabled = false;
			Background3D.WarpColorKey = 0;
		}
	}

	public override void BGM_ON()
	{
	}
}
