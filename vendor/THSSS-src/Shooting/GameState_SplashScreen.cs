using System;
using System.Collections.Generic;
using System.Drawing;
using SlimDX.Direct3D9;

namespace Shooting;

internal class GameState_SplashScreen : IGameState
{
	public TextureObject TxtureObject { get; set; }

	public TextureObject TxtureObject1 { get; set; }

	public GlobalDataPackage GlobalData { get; set; }

	public StageDataPackage StageData { get; set; }

	public string StageName { get; set; }

	public Device DeviceMain => GlobalData.DeviceMain;

	public StateSwitchDataPackage StateSwitchData
	{
		get
		{
			return StageData.StateSwitchData;
		}
		set
		{
			StageData.StateSwitchData = value;
		}
	}

	public MySprite SpriteMain => StageData.SpriteMain;

	public int TimeMain { get; set; }

	public BackgroundManager Background
	{
		get
		{
			return StageData.Background;
		}
		set
		{
			StageData.Background = value;
		}
	}

	public BackgroundManager3D Background3D
	{
		get
		{
			return StageData.Background3D;
		}
		set
		{
			StageData.Background3D = value;
		}
	}

	public ParticleManager3D Particle3D
	{
		get
		{
			return StageData.Particle3D;
		}
		set
		{
			StageData.Particle3D = value;
		}
	}

	public GameState_SplashScreen()
	{
	}

	public GameState_SplashScreen(GlobalDataPackage GlobalData)
	{
		this.GlobalData = GlobalData;
		LoadTexture();
	}

	public void LoadTexture()
	{
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\NowLoading.png");
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\NowLoading-0.png");
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\NowLoading-1.png");
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\Splash_Screen.png");
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\Star.png");
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\sssssstar.png");
		GlobalData.TextureObjectLoader(".\\Image\\NowLoading\\back.png");
		TxtureObject = GlobalData.TextureObjectDictionary["NowLoading-0"];
	}

	public virtual void Init()
	{
		if (StageData == null)
		{
			StageData = new StageDataPackage(GlobalData);
		}
		Background = new BackgroundManager();
		TimeMain = 0;
		Background.Light = 0;
		StageData.EffectList = new List<BaseEffect>();
		StageData.BoundRect = new Rectangle(450, 390, 640, 480);
	}

	public virtual void UpdateData()
	{
		TimeMain++;
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
		if (Background.Light < 0)
		{
			Background.Light = 0;
		}
		else if (Background.Light > 255)
		{
			Background.Light = 255;
		}
		if (TimeMain == 1)
		{
			BaseObject item = new BaseObject(StageData, "Splash_Screen", new PointF(320f, 240f), 0f, Math.PI / 2.0)
			{
				OutsideRegion = 10000
			};
			Background.BackgroundList.Add(item);
			item = new BaseObject(StageData, "sssssstar", new PointF(440f, 160f), 0f, Math.PI / 2.0)
			{
				DirectionVelocityDegree = 0.5f,
				OutsideRegion = 10000
			};
			Background.BackgroundList.Add(item);
			item = new BaseObject(StageData, "back", new PointF(320f, 240f), 0f, Math.PI / 2.0)
			{
				OutsideRegion = 10000
			};
			Background.BackgroundList.Add(item);
		}
		Background.Ctrl();
		new Particle(StageData, "Star", new PointF(StageData.Ran.Next(StageData.BoundRect.Left - 50, StageData.BoundRect.Right), StageData.Ran.Next(StageData.BoundRect.Top + 50, StageData.BoundRect.Top + 100)), StageData.Ran.Next(10, 20) / 10, -Math.PI / 2.0 - (double)((float)StageData.Ran.Next(1, 5) / 10f))
		{
			Active = true,
			Scale = (float)StageData.Ran.Next(25, 70) / 100f,
			LifeTime = 70
		};
		if (GlobalData.LoadingList.Count > 0)
		{
			GlobalData.TextureObjectLoader(GlobalData.LoadingList[0]);
			GlobalData.LoadingList.RemoveAt(0);
		}
	}

	public virtual void Render()
	{
		//IL_009c: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ab: Unknown result type (might be due to invalid IL or missing references)
		for (int num = StageData.EffectList.Count - 1; num >= 0; num--)
		{
			StageData.EffectList[num].Ctrl();
		}
		SpriteMain.Begin((SpriteFlags)16);
		Background.Show(Active: false);
		StageData.EffectList.ForEach((BaseEffect x) =>
		{
			if (!x.Active)
			{
				x.Show();
			}
		});
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		SpriteMain.Draw2D(TxtureObject, 1f, 0f, new Point(520, 432), Background.Light);
		StageData.EffectList.ForEach((BaseEffect x) =>
		{
			if (x.Active)
			{
				x.Show();
			}
		});
		SpriteMain.End();
	}

	public void BGM_ON()
	{
	}

	public virtual void BGM_Resume()
	{
	}

	public virtual void BGM_Pause()
	{
	}

	public void Dispose()
	{
		if (StageData != null)
		{
			StageData.Dispose();
		}
	}
}
