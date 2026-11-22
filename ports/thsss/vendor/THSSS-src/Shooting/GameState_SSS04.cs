using System.Drawing;
using System.IO;
using Shooting.Planes.Story;

namespace Shooting;

internal class GameState_SSS04 : BaseStage, IGameState
{
	public GameState_SSS04(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "St4";
	}

	public override void Drama()
	{
		base.Drama();
		int[] array = new int[3] { 0, 0, 8850 };
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
			if (StageData.C_History.PracticeLevel < 4)
			{
				StageData.C_History.PracticeLevel = 4;
			}
			StageData.ChangeBGM(".\\BGM\\Stage04.wav", (testStartTime + 60) * 44100 / 60, 0, 255, 575990, 7775976);
			new MusicTitle(StageData, "万花世界的光与影", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Background3D.Envi = new EnvironmentSSS04(StageData);
			new BackgroundTransitionIn(StageData)
			{
				Delay = 20,
				LifeTime = 150
			};
			if (testStartTime > TimeMain)
			{
				TimeMain = testStartTime;
			}
			if (TimeMain < array[2])
			{
				string[] array2 = (int)StageData.Difficulty switch
				{
					0 => Directory.GetFiles(".\\CS\\St04\\A\\E\\", "*.mbg"), 
					1 => Directory.GetFiles(".\\CS\\St04\\A\\N\\", "*.mbg"), 
					2 => Directory.GetFiles(".\\CS\\St04\\A\\H\\", "*.mbg"), 
					3 => Directory.GetFiles(".\\CS\\St04\\A\\L\\", "*.mbg"), 
					_ => Directory.GetFiles(".\\CS\\St04\\A\\L\\", "*.mbg"), 
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
		if (array[2] - 130 == TimeMain)
		{
			new BossStar(StageData, new PointF(BoundRect.Width / 2, 150f)).ColorType = 2;
		}
		else if (array[2] == TimeMain)
		{
			StageData.RemoveBullets();
			new Boss_Kage01(StageData);
			new Story_SSS04_01(StageData);
			Background3D.WarpEnabled = false;
			Background3D.WarpColorKey = 0;
		}
	}

	public override void BGM_ON()
	{
	}
}
