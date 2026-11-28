using System.Drawing;
using System.IO;
using Shooting.Planes.Story;
using SlimDX.Direct3D9;

namespace Shooting;

internal class GameState_ED : BaseGameState, IGameState
{
	private int testStartTime;

	private bool StageFlag;

	private int[] keyTime = new int[1] { 500 };

	public GameState_ED(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		//IL_0037: Unknown result type (might be due to invalid IL or missing references)
		//IL_003d: Expected Obj, but got Unknown
		//IL_0045: Unknown result type (might be due to invalid IL or missing references)
		//IL_004f: Expected Obj, but got Unknown
		StageName = "All";
		Font val = new Font("Arial", 14f * GlobalDataPackage.FontScale, (FontStyle)3);
		DXFont = new Font(DeviceMain, val);
	}

	public override void UpdateData()
	{
		TimeMain++;
		SoundPlayList.Clear();
		if (StageData.Rep != null && StageData.OnReplay)
		{
			int num = StageData.Rep.ReadKey();
			if (num == 57358)
			{
				num = StageData.Rep.ReadKey();
			}
			if (num == 4080)
			{
				StateSwitchData = new StateSwitchDataPackage
				{
					NextState = "GameOverMenu",
					NeedInit = true,
					SDPswitch = new StageDataPackage(GlobalData)
				};
				GlobalData.LastState = this;
				StageData.KClass.Hex2Key(0);
				StageData.SoundPlay("se_pause.wav");
			}
			else
			{
				StageData.KClass.Hex2Key(num);
			}
		}
		Background3D.Ctrl();
		Background.Ctrl();
		Background2.Ctrl();
		Drama();
		if (Story != null)
		{
			Story.Ctrl();
		}
		for (int num2 = EnemyPlaneList.Count - 1; num2 >= 0; num2--)
		{
			EnemyPlaneList[num2].Ctrl();
		}
		for (int num3 = BulletList.Count - 1; num3 >= 0; num3--)
		{
			BulletList[num3].Ctrl();
		}
		for (int num4 = EffectList.Count - 1; num4 >= 0; num4--)
		{
			EffectList[num4].Ctrl();
		}
		Particle3D.Ctrl();
		for (int num5 = InterfaceList.Count - 1; num5 >= 0; num5--)
		{
			InterfaceList[num5].Ctrl();
		}
		StageData.Vibrate();
		SoundPlayList.ForEach((XAudio2_Player x) =>
		{
			x.Play();
		});
		if (TimeMain % 6 != 0 && KClass.Key_plus && MyPlane.Life >= 0 && StateSwitchData == null)
		{
			UpdateData();
		}
		GlobalData.LastState = this;
	}

	public override void Init()
	{
		StageData.C_History = StageData.PData.C_History.Find(StageData.FindClear);
	}

	public void Drama()
	{
		if (TimeMain == 2)
		{
			if (StageData.C_History.PracticeLevel < 7)
			{
				StageData.C_History.PracticeLevel = 7;
			}
			StageFlag = false;
			new BackgroundTransitionIn(StageData)
			{
				Delay = 0,
				LifeTime = 280
			};
			BoundRect = new Rectangle(128, 0, 384, 480);
			if (StageData.ContinueTimes == 0)
			{
				StageData.ChangeBGM(".\\BGM\\ED.wav", 0, 0, 0, 0, 0);
				new Story_SSStaff(StageData);
			}
			StageData.CurrentStageName = "All";
			StageData.RepInfo.LastStage = StageData.CurrentStageName;
			StageData.C_History = StageData.PData.C_History.Find(StageData.FindClear);
			StageData.C_History.ClearTimes++;
			if (StageData.ContinueTimes == 0)
			{
				StageData.C_History.NoContinueClearTimes++;
			}
			StageData.S_History = StageData.PData.S_History.FindAll(StageData.FindScore);
		}
		if (TimeMain > 2 && Story == null && !StageFlag)
		{
			StageFlag = true;
			TimeMain = 3;
		}
		if (Story != null)
		{
			return;
		}
		if (TimeMain == 5)
		{
			StageData.ChangeBGM(".\\BGM\\Staff.wav", 0, 0, 0, 0, 0);
			string[] files = Directory.GetFiles(".\\CS\\Staff\\", "*.mbg");
			foreach (string fileName in files)
			{
				CS_Data cSData = StageData.LoadCS(fileName);
				new CSEmitterController(StageData, cSData)
				{
					OnRoad = true,
					Time = TimeMain - 60
				};
			}
			BoundRect = new Rectangle(128, 0, 640, 480);
		}
		if (!StageFlag || TimeMain <= keyTime[0] || !KClass.Key_Z)
		{
			return;
		}
		if (StageData.ContinueTimes == 0)
		{
			EndStage ed = new EndStage(StageData, "FinalReplaySaveMenu", NextInit: true);
			ed.CtrlCall = (string x) =>
			{
				if (ed.Time == ed.LifeTime)
				{
					BoundRect = new Rectangle(0, 0, 640, 480);
				}
			};
			GlobalData.LastState = this;
			if (StageData.C_History.PracticeLevel < 8)
			{
				StageData.C_History.PracticeLevel = 8;
			}
		}
		else
		{
			new EndStage(StageData, "MainMenu", NextInit: true);
			GlobalData.LastState = this;
		}
	}

	public override void BGM_ON()
	{
	}

	public override void Render()
	{
		//IL_0026: Unknown result type (might be due to invalid IL or missing references)
		//IL_0035: Unknown result type (might be due to invalid IL or missing references)
		//IL_008c: Unknown result type (might be due to invalid IL or missing references)
		//IL_009b: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a6: Unknown result type (might be due to invalid IL or missing references)
		//IL_0307: Unknown result type (might be due to invalid IL or missing references)
		//IL_0318: Unknown result type (might be due to invalid IL or missing references)
		GlobalData.ScreenTexMan.Begin();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		Background2.Show(Active: true);
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		Background2.Show(Active: false);
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			if (x.Active)
			{
				x.Show();
			}
		});
		EffectList.ForEach((BaseEffect x) =>
		{
			if (x.Active)
			{
				x.Show();
			}
		});
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			if (!x.Active && x.Layer < 0)
			{
				x.Show();
			}
		});
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			if (!x.Active && x.Layer == 0)
			{
				x.Show();
			}
		});
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			if (!x.Active && x.Layer > 0)
			{
				x.Show();
			}
		});
		EffectList.ForEach((BaseEffect x) =>
		{
			if (!x.Active && x.Layer < 0)
			{
				x.Show();
			}
		});
		EffectList.ForEach((BaseEffect x) =>
		{
			if (!x.Active && x.Layer == 0)
			{
				x.Show();
			}
		});
		EffectList.ForEach((BaseEffect x) =>
		{
			if (!x.Active && x.Layer > 0)
			{
				x.Show();
			}
		});
		if (Story != null)
		{
			Story.Show();
		}
		SpriteMain.End();
		Particle3D.Show();
		GlobalData.ScreenTexMan.End();
		SpriteMain.Begin((SpriteFlags)16);
		GlobalData.EffectMain = GlobalData.EffectDictionary["Filter"];
		GlobalData.EffectMain.Technique = EffectHandle.op_Implicit("technique1");
		GlobalData.EffectMain.Begin();
		GlobalData.EffectMain.BeginPass(0);
		GlobalData.SpriteMain.Draw2D(GlobalData.ScreenTexMan.RenderTexture, new Point(0, 0), 0f, new PointF(0f, 0f), Color.White);
		SpriteMain.End();
		GlobalData.EffectMain.EndPass();
		GlobalData.EffectMain.End();
	}
}
