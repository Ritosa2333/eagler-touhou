using System;
using System.Drawing;
using System.IO;
using Shooting.Planes.Story;

namespace Shooting;

internal class GameState_SSSEx : BaseStage, IGameState
{
	private new int testStartTime;

	public GameState_SSSEx(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "StEx";
	}

	public override void Drama()
	{
		base.Drama();
		int[] array = new int[3] { 4920, 5060, 10260 };
		if (TimeMain == 1)
		{
			MyPlane.EnchantmentCountNeeded = 10;
		}
		if (TimeMain == 62)
		{
			if (StageData.C_History.PracticeLevel < 8)
			{
				StageData.C_History.PracticeLevel = 8;
			}
			StageData.ChangeBGM(".\\BGM\\StageEx.wav", (testStartTime + 60) * 44100 / 60, 0, 255, 1341963, 7621362);
			new MusicTitle(StageData, "来自仙界的新风", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Background3D.Envi = new EnvironmentSSSEx(StageData);
			new BackgroundTransitionIn(StageData)
			{
				Delay = 0,
				LifeTime = 320
			};
			if (testStartTime > TimeMain)
			{
				TimeMain = testStartTime;
			}
			if (TimeMain < array[0])
			{
				string[] files = Directory.GetFiles(".\\CS\\StEx\\A\\", "*.mbg");
				foreach (string fileName in files)
				{
					CS_Data cSData = StageData.LoadCS(fileName);
					new CSEmitterController(StageData, cSData)
					{
						OnRoad = true,
						Time = TimeMain - 60
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
		if (TimeMain == array[0])
		{
			StageData.RemoveBullets();
			new Boss_Seiryuu03(StageData);
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(40, 255, 20));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
			Background3D.WarpColorKey = 2;
		}
		else if (TimeMain > array[1] && Boss == null && !RoadFlag)
		{
			RoadFlag = true;
			string[] files = Directory.GetFiles(".\\CS\\StEx\\B\\", "*.mbg");
			foreach (string fileName2 in files)
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
			new Boss_Rika01(StageData);
			new Story_SSSEx_01(StageData);
			Background3D.WarpEnabled = false;
			Background3D.WarpColorKey = 2;
		}
	}

	public override void BGM_ON()
	{
	}
}
