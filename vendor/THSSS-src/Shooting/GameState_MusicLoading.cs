using System;
using System.Drawing;
using System.Threading;
using SlimDX.Direct3D9;

namespace Shooting;

internal class GameState_MusicLoading : BaseMenu, IGameState
{
	private bool LoadFlag;

	private Thread LoadingThread;

	public GameState_MusicLoading(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
	}

	public GameState_MusicLoading()
	{
	}

	public override void Init()
	{
		LoadFlag = false;
		Background.Light = 0;
	}

	public override void UpdateData()
	{
		base.UpdateData();
		if (TimeMain == 1)
		{
			LoadingThread = new Thread(PreloadMusic);
			LoadingThread.Start();
		}
		if (LoadFlag && TimeMain > 0)
		{
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = StageData.GlobalData.LastState.StageName,
				NeedInit = false
			};
			((BaseGameState)StageData.GlobalData.LastState).TimeSycn();
			LoadingThread.Abort();
		}
		if (TimeMain < 50)
		{
			Background.Light += 5;
		}
		else if (TimeMain > 60)
		{
			if (TimeMain % 60 >= 30)
			{
				Background.Light += 4;
			}
			else if (TimeMain % 60 < 30)
			{
				Background.Light -= 4;
			}
		}
		new Particle(StageData, "Star", new PointF(StageData.Ran.Next(400, 640), StageData.Ran.Next(440, 580)), StageData.Ran.Next(10, 20) / 10, -Math.PI / 2.0 - (double)((float)StageData.Ran.Next(1, 5) / 10f))
		{
			Active = true,
			Scale = (float)StageData.Ran.Next(25, 70) / 100f,
			LifeTime = 70
		};
	}

	private void PreloadMusic()
	{
		if (GlobalData.LastState is BaseStage)
		{
			switch (((BaseStage)GlobalData.LastState).StageName)
			{
			case "St1":
				BGM_Player.PreLoad(".\\BGM\\Stage01.wav");
				BGM_Player.PreLoad(".\\BGM\\Boss01.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\St01");
				break;
			case "St2":
				BGM_Player.PreLoad(".\\BGM\\Stage02.wav");
				BGM_Player.PreLoad(".\\BGM\\Boss02.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\St02");
				break;
			case "St3":
				BGM_Player.PreLoad(".\\BGM\\Stage03.wav");
				BGM_Player.PreLoad(".\\BGM\\Boss03.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\St03");
				break;
			case "St4":
				BGM_Player.PreLoad(".\\BGM\\Stage04.wav");
				BGM_Player.PreLoad(".\\BGM\\Boss04.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\St04");
				break;
			case "St5":
				BGM_Player.PreLoad(".\\BGM\\Stage05.wav");
				BGM_Player.PreLoad(".\\BGM\\Boss05.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\St05");
				break;
			case "St6":
				BGM_Player.PreLoad(".\\BGM\\Stage06.wav");
				BGM_Player.PreLoad(".\\BGM\\Boss06.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\St06");
				break;
			case "StEx":
				BGM_Player.PreLoad(".\\BGM\\StageEx.wav");
				BGM_Player.PreLoad(".\\BGM\\BossEx.wav");
				GlobalData.LastState.StageData.ClearCSCathe();
				GlobalData.LastState.StageData.PreLoadCS(".\\CS\\StEx");
				break;
			}
		}
		LoadFlag = true;
	}

	public override void Render()
	{
		//IL_0026: Unknown result type (might be due to invalid IL or missing references)
		//IL_0035: Unknown result type (might be due to invalid IL or missing references)
		GlobalData.LastState.Render();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		EffectList.ForEach((BaseEffect x) =>
		{
			x.Show();
		});
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		SpriteMain.Draw2D(GlobalData.TextureObjectDictionary["NowLoading-0"], 1f, 0f, new Point(520, 440), Background.Light);
		StageData.EffectList.ForEach((BaseEffect x) =>
		{
			if (!x.Active)
			{
				x.Show();
			}
		});
		SpriteMain.End();
	}

	public override void BGM_Pause()
	{
	}

	public override void BGM_Resume()
	{
	}
}
