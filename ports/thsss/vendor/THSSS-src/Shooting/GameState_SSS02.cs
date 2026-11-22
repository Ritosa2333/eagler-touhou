using System.Drawing;
using System.IO;
using Shooting.Planes.Story;

namespace Shooting;

internal class GameState_SSS02 : BaseStage, IGameState
{
	public GameState_SSS02(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "St2";
	}

	public override void Drama()
	{
		base.Drama();
		int[] array = new int[3] { 4680, 5000, 7890 };
		if (TimeMain == 1)
		{
			switch ((int)StageData.Difficulty)
			{
			case 2:
				MyPlane.EnchantmentCountNeeded = 5;
				break;
			case 3:
				MyPlane.EnchantmentCountNeeded = 6;
				break;
			}
		}
		if (TimeMain == 2)
		{
			if (StageData.C_History.PracticeLevel < 2)
			{
				StageData.C_History.PracticeLevel = 2;
			}
			StageData.ChangeBGM(".\\BGM\\Stage02.wav", (testStartTime + 60) * 44100 / 60, 0, 255, 1194228, 6866370);
			new MusicTitle(StageData, "灯火竹林", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Background3D.Envi = new EnvironmentSSS02(StageData);
			new BackgroundTransitionIn(StageData)
			{
				Delay = 200,
				LifeTime = 320
			};
			if (testStartTime > TimeMain)
			{
				TimeMain = testStartTime;
			}
			if (TimeMain < array[0])
			{
				string[] array2 = (int)StageData.Difficulty switch
				{
					0 => Directory.GetFiles(".\\CS\\St02\\A\\E\\", "*.mbg"), 
					1 => Directory.GetFiles(".\\CS\\St02\\A\\N\\", "*.mbg"), 
					2 => Directory.GetFiles(".\\CS\\St02\\A\\H\\", "*.mbg"), 
					3 => Directory.GetFiles(".\\CS\\St02\\A\\L\\", "*.mbg"), 
					_ => Directory.GetFiles(".\\CS\\St02\\A\\L\\", "*.mbg"), 
				};
				foreach (string fileName in array2)
				{
					CS_Data cSData = StageData.LoadCS(fileName);
					new CSEmitterController(StageData, cSData)
					{
						OnRoad = true,
						Time = TimeMain - 60
					};
				}
			}
		}
		if (TimeMain == array[0])
		{
			StageData.RemoveBullets();
			new Boss_Rakuki01(StageData);
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(144, 255, 120));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
			Background3D.WarpColorKey = 2;
		}
		else if (TimeMain > array[1] && Boss == null && !RoadFlag)
		{
			RoadFlag = true;
			string[] array2 = (int)StageData.Difficulty switch
			{
				0 => Directory.GetFiles(".\\CS\\St02\\B\\E\\", "*.mbg"), 
				1 => Directory.GetFiles(".\\CS\\St02\\B\\N\\", "*.mbg"), 
				2 => Directory.GetFiles(".\\CS\\St02\\B\\H\\", "*.mbg"), 
				3 => Directory.GetFiles(".\\CS\\St02\\B\\L\\", "*.mbg"), 
				_ => Directory.GetFiles(".\\CS\\St02\\B\\L\\", "*.mbg"), 
			};
			foreach (string fileName2 in array2)
			{
				CS_Data cSData2 = StageData.LoadCS(fileName2);
				new CSEmitterController(StageData, cSData2)
				{
					OnRoad = true,
					Time = TimeMain - 60
				};
			}
		}
		else if (array[2] - 130 == TimeMain)
		{
			new BossStar(StageData, new PointF(BoundRect.Width / 2, 120f)).ColorType = 5;
		}
		else if (array[2] == TimeMain)
		{
			StageData.RemoveBullets();
			new Boss_Rakuki02(StageData);
			switch (MyPlane.Name)
			{
			case "Aya":
				new Story_SSS02_01(StageData);
				break;
			case "Reimu":
				new Story_SSS02_01A(StageData);
				break;
			case "Marisa":
				new Story_SSS02_01B(StageData);
				break;
			case "Sanae":
				new Story_SSS02_01C(StageData);
				break;
			case "Koishi":
				new Story_SSS02_01D(StageData);
				break;
			default:
				new Story_SSS02_01X(StageData);
				break;
			}
			Background3D.WarpEnabled = false;
			Background3D.WarpColorKey = 2;
		}
	}

	public override void BGM_ON()
	{
	}
}
