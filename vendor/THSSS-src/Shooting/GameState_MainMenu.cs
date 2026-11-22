using System;
using System.Drawing;
using System.Windows.Forms;
using Shooting.Properties;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class GameState_MainMenu : BaseMenu, IGameState
{
	private GameRegion GameRegion_Menu;

	private BaseEffect SelectFrame;

	private void InitializeComponent()
	{
		GameRegion_Menu = new GameRegion();
		((Control)this).SuspendLayout();
		((Control)GameRegion_Menu).AutoSize = true;
		((Control)GameRegion_Menu).BackColor = Color.Transparent;
		((Control)GameRegion_Menu).BackgroundImage = (Image)(object)Resources.Menu_Start;
		((Control)GameRegion_Menu).BackgroundImageLayout = (ImageLayout)2;
		((Control)GameRegion_Menu).Enabled = false;
		((Control)GameRegion_Menu).Location = new Point(59, 138);
		((Control)GameRegion_Menu).Name = "GameRegion_Menu";
		((Control)GameRegion_Menu).Size = new Size(103, 28);
		((Control)GameRegion_Menu).TabIndex = 0;
		((Control)GameRegion_Menu).Visible = false;
		((Control)this).BackgroundImage = (Image)(object)Resources.菜单背景;
		((Control)this).Controls.Add((Control)(object)GameRegion_Menu);
		((Control)this).Enabled = false;
		((Control)this).Name = "GameState_MainMenu";
		((Control)this).Size = new Size(640, 480);
		((Control)this).ResumeLayout(false);
		((Control)this).PerformLayout();
	}

	public GameState_MainMenu(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		//IL_001e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0024: Expected Obj, but got Unknown
		//IL_002c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0036: Expected Obj, but got Unknown
		InitializeComponent();
		Font val = new Font("Arial", 12f * GlobalDataPackage.FontScale, (FontStyle)3);
		DXFont = new Font(DeviceMain, val);
	}

	public override void Init()
	{
		StageData.PData.CheckEnable();
		StageData.PData.SaveData();
		BackgroundLight = 0;
		BGM_Player.Volume = StageData.GlobalData.BGMVolume;
	}

	public override void UpdateData()
	{
		if (TimeMain == 50)
		{
			new BaseEffect(StageData, "woshixingtu", new PointF(385f, 240f), 4f, 0.0)
			{
				AngleDegree = 90.0,
				Active = false,
				DestPoint = new PointF(320f, 240f),
				AngleWithDirection = false,
				Scale = 1f,
				TransparentValueF = 0f,
				TransparentVelocity = 5f
			};
		}
		else if (TimeMain == 100)
		{
			new BaseEffect(StageData, "woshibiaotihebaisi", new PointF(385f, 240f), 4f, 0.0)
			{
				AngleDegree = 90.0,
				Active = false,
				DestPoint = new PointF(320f, 240f),
				AngleWithDirection = false,
				Scale = 1f,
				TransparentValueF = 0f,
				TransparentVelocity = 5f
			};
			new EmitterTitleStarFall(StageData);
			StageData.MenuGroupList.Add(new MenuGroup_Main(StageData));
		}
		else if (TimeMain == 130)
		{
			SelectFrame = new SelectBox(StageData)
			{
				Velocity = 100f,
				OriginalPosition = new PointF(0f, 500f)
			};
		}
		if (StageData.MenuGroupList.Count > 0)
		{
			BaseMenuItem baseMenuItem = StageData.MenuGroupList[StageData.MenuGroupList.Count - 1].MenuItemList[StageData.MenuGroupList[StageData.MenuGroupList.Count - 1].MenuSelectIndex];
			if (StageData.MenuGroupList.Count == 1 || StageData.MenuGroupList[StageData.MenuGroupList.Count - 1] is MenuOption)
			{
				new Particle(StageData, "Star", new PointF(Ran.Next((int)baseMenuItem.OriginalPosition.X - 10, (int)baseMenuItem.OriginalPosition.X + baseMenuItem.TxtureObject.PosRect.Width + 10), baseMenuItem.OriginalPosition.Y + 20f), StageData.Ran.Next(10, 20) / 10, -Math.PI / 2.0 - (double)((float)StageData.Ran.Next(1, 5) / 10f))
				{
					Active = true,
					Scale = (float)StageData.Ran.Next(15, 40) / 100f,
					LifeTime = 20
				};
			}
			if (SelectFrame != null)
			{
				baseMenuItem = StageData.MenuGroupList[0].MenuItemList[StageData.MenuGroupList[0].MenuSelectIndex];
				if (Math.Abs(SelectFrame.DestPoint.Y - baseMenuItem.OriginalPosition.Y - 6f) >= 1f)
				{
					SelectFrame.Velocity = Math.Abs(SelectFrame.DestPoint.Y - baseMenuItem.OriginalPosition.Y - 6f) / 5f;
					if (SelectFrame.Velocity > 16f)
					{
						SelectFrame.Velocity = 16f;
					}
					else if (SelectFrame.Velocity < 2f)
					{
						SelectFrame.Velocity = 2f;
					}
				}
				SelectFrame.DestPoint = new PointF(0f, baseMenuItem.OriginalPosition.Y + 6f);
			}
		}
		base.UpdateData();
		if (0 <= TimeMain && TimeMain < 130)
		{
			BackgroundLight += 4;
			if (BackgroundLight > 255)
			{
				BackgroundLight = 255;
			}
		}
		else
		{
			BackgroundLight = 255;
		}
	}

	public override void Render()
	{
		//IL_006b: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a2: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b1: Unknown result type (might be due to invalid IL or missing references)
		//IL_0161: Unknown result type (might be due to invalid IL or missing references)
		//IL_0170: Unknown result type (might be due to invalid IL or missing references)
		SpriteMain.Begin((SpriteFlags)16);
		SpriteMain.Draw2D(TextureObjectDictionary["MenuBackground"], 0.625f, 0f, new PointF(320f, 240f), 255);
		DXFont.DrawString(SpriteMain.sprite, "ver 1.01", 560, 460, Color4.op_Implicit(Color.Black));
		SpriteMain.End();
		Background3D.Show();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		Background2.Show(Active: true);
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		Background2.Show(Active: false);
		EffectList.ForEach((BaseEffect x) =>
		{
			if (!x.Active)
			{
				x.Show();
			}
		});
		StageData.MenuGroupList.ForEach((BaseMenuGroup x) =>
		{
			x.Show();
		});
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		EffectList.ForEach((BaseEffect x) =>
		{
			if (x.Active)
			{
				x.Show();
			}
		});
		SpriteMain.End();
		Particle3D.Show();
		if (TimeMain < 126)
		{
			GlobalData.ScreenTexMan.Begin();
			GlobalData.LastState.Render();
			GlobalData.ScreenTexMan.End();
			SpriteMain.Begin((SpriteFlags)16);
			GlobalData.SpriteMain.Draw2D(GlobalData.ScreenTexMan.RenderTexture, new Point(0, 0), 0f, new PointF(0f, 0f), Color.FromArgb(255 - TimeMain * 2, Color.White));
			SpriteMain.End();
		}
	}

	public override void BGM_ON()
	{
		StageData.ChangeBGM(".\\BGM\\OP.wav", 0, 0, 255, 604611, 6047874);
	}

	public override void BGM_Resume()
	{
	}
}
