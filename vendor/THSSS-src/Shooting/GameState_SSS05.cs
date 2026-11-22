using System.Drawing;
using System.IO;
using Shooting.Planes.Story;

namespace Shooting;

internal class GameState_SSS05 : BaseStage, IGameState
{
	public GameState_SSS05(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "St5";
	}

	public override void Drama()
	{
		base.Drama();
		int[] array = new int[3] { 2500, 3000, 8750 };
		if (TimeMain == 1)
		{
			switch ((int)StageData.Difficulty)
			{
			case 2:
				MyPlane.EnchantmentCountNeeded = 8;
				break;
			case 3:
				MyPlane.EnchantmentCountNeeded = 9;
				break;
			}
		}
		if (TimeMain == 2)
		{
			if (StageData.C_History.PracticeLevel < 5)
			{
				StageData.C_History.PracticeLevel = 5;
			}
			StageData.ChangeBGM(".\\BGM\\Stage05.wav", (testStartTime + 60) * 44100 / 60, 0, 255, 66150, 0);
			new MusicTitle(StageData, "记忆中遥远的星星", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Background3D.Envi = new EnvironmentSSS05(StageData);
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
					0 => Directory.GetFiles(".\\CS\\St05\\A\\E\\", "*.mbg"), 
					1 => Directory.GetFiles(".\\CS\\St05\\A\\N\\", "*.mbg"), 
					2 => Directory.GetFiles(".\\CS\\St05\\A\\H\\", "*.mbg"), 
					3 => Directory.GetFiles(".\\CS\\St05\\A\\L\\", "*.mbg"), 
					_ => Directory.GetFiles(".\\CS\\St05\\A\\L\\", "*.mbg"), 
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
			new Boss_Rakuki04(StageData);
			StageData.RemoveBullets();
		}
		else if (TimeMain == array[0] + 90)
		{
			new Story_SSS05_01(StageData);
		}
		else if (TimeMain > array[1] && Boss == null && !RoadFlag)
		{
			RoadFlag = true;
			string[] array2 = (int)StageData.Difficulty switch
			{
				0 => Directory.GetFiles(".\\CS\\St05\\B\\E\\", "*.mbg"), 
				1 => Directory.GetFiles(".\\CS\\St05\\B\\N\\", "*.mbg"), 
				2 => Directory.GetFiles(".\\CS\\St05\\B\\H\\", "*.mbg"), 
				3 => Directory.GetFiles(".\\CS\\St05\\B\\L\\", "*.mbg"), 
				_ => Directory.GetFiles(".\\CS\\St05\\B\\L\\", "*.mbg"), 
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
		if (array[2] - 130 == TimeMain)
		{
			new BossStar(StageData, new PointF(BoundRect.Width / 2, 150f)).ColorType = 1;
		}
		else if (array[2] == TimeMain)
		{
			StageData.RemoveBullets();
			new Boss_Rakukun01(StageData);
			new Story_SSS05_02(StageData);
			Background3D.WarpEnabled = false;
			Background3D.WarpColorKey = 0;
		}
	}

	public override void BGM_ON()
	{
	}
}
