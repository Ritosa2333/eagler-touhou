using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Shooting.Properties;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BaseStage : BaseGameState, IGameState
{
	protected int testStartTime;

	private string testString;

	private IContainer components;

	private GameRegion GameRegion_Main;

	private GameRegion gameRegion_Score;

	private GameRegion gameRegion_Player;

	private GameRegion gameRegion_Spell;

	private GameRegion gameRegion_Power;

	private GameRegion gameRegion_LifeSelf;

	private GameRegion gameRegion_SpellLine;

	private GameRegion gameRegion_ScoreValue;

	private GameRegion gameRegion_BluePoint;

	private GameRegion gameRegion_GreenPoint;

	private GameRegion gameRegion_Graze;

	private GameRegion gameRegion_HiScoreValue;

	private GameRegion gameRegion1;

	private GameRegion gameRegion_HiScore;

	private byte Slowflag { get; set; }

	protected bool RoadFlag { get; set; }

	public BaseStage(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		//IL_001e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0024: Expected Obj, but got Unknown
		//IL_002c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0036: Expected Obj, but got Unknown
		InitializeComponent();
		Font val = new Font("Arial", 14f * GlobalDataPackage.FontScale, (FontStyle)1);
		DXFont = new Font(DeviceMain, val);
	}

	public override void Init()
	{
		BoundRect = new Rectangle(((Control)GameRegion_Main).Location, ((Control)GameRegion_Main).Size);
		Point originalPosition = new Point(BoundRect.Width / 2, BoundRect.Bottom - 50);
		if (MyPlane == null)
		{
			MyPlane = new MyPlane_Aya(StageData, originalPosition);
		}
		Boss = null;
		BulletList = new List<BaseBullet_Touhou>();
		EnemyPlaneList = new List<BaseEnemyPlane>();
		MyBulletList = new List<BaseObject>();
		SpellList = new List<BaseObject>();
		EffectList = new List<BaseEffect>();
		ItemList = new List<BaseItem>();
		InterfaceList = new List<BaseObject>
		{
			new StarPointLineG(StageData, new PointF(66f, 404f)),
			new StarPointLineB(StageData, new PointF(-6f, 404f)),
			new StarPointLineR(StageData, new PointF(-6f, 404f)),
			new BaseInterface(StageData, "TotalBackgroundHalf", new Point(0, 0))
			{
				Delay = 0
			},
			new BossLife_Line(StageData, "表示符卡数量的星星（单个）", new PointF(BoundRect.Left + 65, BoundRect.Top + 22)),
			new HP_Line(StageData, "HP_Line1", new PointF(BoundRect.Left + BoundRect.Width / 2 + 18, BoundRect.Top + 13)),
			new SpellClock(StageData),
			new MyLife_Line(StageData, MyPlane.TxtureObject, ((Control)gameRegion_LifeSelf).Location)
			{
				Delay = 30
			},
			new Item_Line_LifeChip(StageData, new Point(((Control)gameRegion_LifeSelf).Location.X + 82, ((Control)gameRegion_LifeSelf).Location.Y + 11))
			{
				Delay = 35
			},
			new MySpell_Line(StageData, "Bomb", ((Control)gameRegion_SpellLine).Location)
			{
				Delay = 40
			},
			new Item_Line_SpellChip(StageData, new Point(((Control)gameRegion_SpellLine).Location.X + 82, ((Control)gameRegion_SpellLine).Location.Y + 11))
			{
				Delay = 45
			},
			new BaseInterface(StageData, "最高得分", ((Control)gameRegion_HiScore).Location)
			{
				Delay = 10
			},
			new BaseInterface(StageData, "InterfaceLine00", new Point(((Control)gameRegion_HiScore).Location.X + 60, ((Control)gameRegion_HiScore).Location.Y + 16))
			{
				Delay = 15
			},
			new BaseInterface(StageData, "得分", ((Control)gameRegion_Score).Location)
			{
				Delay = 20
			},
			new BaseInterface(StageData, "InterfaceLine01", new Point(((Control)gameRegion_Score).Location.X + 60, ((Control)gameRegion_Score).Location.Y + 16))
			{
				Delay = 25
			},
			new BaseInterface(StageData, "剩余人数", ((Control)gameRegion_Player).Location)
			{
				Delay = 30
			},
			new BaseInterface(StageData, "InterfaceLine02", new Point(((Control)gameRegion_Player).Location.X + 60, ((Control)gameRegion_Player).Location.Y + 22))
			{
				Delay = 35
			},
			new BaseInterface(StageData, "SC", ((Control)gameRegion_Spell).Location)
			{
				Delay = 40
			},
			new BaseInterface(StageData, "InterfaceLine03", new Point(((Control)gameRegion_Spell).Location.X + 60, ((Control)gameRegion_Spell).Location.Y + 22))
			{
				Delay = 45
			},
			new BaseInterface(StageData, "灵力", ((Control)gameRegion_Power).Location)
			{
				Delay = 70
			},
			new BaseInterface(StageData, "InterfaceLine04", new Point(((Control)gameRegion_Spell).Location.X + 60, ((Control)gameRegion_Power).Location.Y + 16))
			{
				Delay = 75
			},
			new BaseInterface(StageData, "最大得点", ((Control)gameRegion_BluePoint).Location)
			{
				Delay = 80
			},
			new BaseInterface(StageData, "InterfaceLine05", new Point(((Control)gameRegion_Spell).Location.X + 60, ((Control)gameRegion_BluePoint).Location.Y + 16))
			{
				Delay = 85
			},
			new BaseInterface(StageData, "擦弹", new Point(((Control)gameRegion_Graze).Location.X, ((Control)gameRegion_GreenPoint).Location.Y))
			{
				Delay = 90
			},
			new Item_Line_HiScore(StageData, new Point(((Control)gameRegion_HiScoreValue).Location.X - 20, ((Control)gameRegion_HiScoreValue).Location.Y))
			{
				Delay = 10
			},
			new Item_Line_Score(StageData, new Point(((Control)gameRegion_ScoreValue).Location.X - 20, ((Control)gameRegion_ScoreValue).Location.Y))
			{
				Delay = 20
			},
			new Item_Line_Red(StageData, new Point(((Control)gameRegion_Power).Location.X + 50, ((Control)gameRegion_Power).Location.Y))
			{
				Delay = 70
			},
			new Item_Line_Blue(StageData, new Point(((Control)gameRegion_BluePoint).Location.X + 50, ((Control)gameRegion_BluePoint).Location.Y))
			{
				Delay = 80
			},
			new Item_Line_Gray(StageData, new Point(((Control)gameRegion_Graze).Location.X + 50, ((Control)gameRegion_GreenPoint).Location.Y))
			{
				Delay = 90
			},
			new BossMark(StageData),
			new InterfaceDifficulty(StageData, new PointF((BoundRect.Right + 640) / 2, 24f))
			{
				Delay = 110
			}
		};
		if (StageData.Difficulty > DifficultLevel.Normal)
		{
			InterfaceList.Add(new BaseInterface(StageData, "BD", new Point(((Control)gameRegion_Spell).Location.X, ((Control)gameRegion_Spell).Location.Y + 26))
			{
				Delay = 50
			});
			InterfaceList.Add(new MyBorder_Line(StageData, "Border", new Point(((Control)gameRegion_SpellLine).Location.X, ((Control)gameRegion_SpellLine).Location.Y + 26))
			{
				Delay = 50
			});
		}
		SoundPlayList = new List<XAudio2_Player>();
		Background = new BackgroundManager();
		Background3D = new BackgroundManager3D(StageData);
		Particle3D = new ParticleManager3D(StageData);
		TimeMain = 0;
		StageData.Ran = new MyRandom(0);
		StageData.C_History = StageData.PData.C_History.Find(StageData.FindClear);
		StageData.S_History = StageData.PData.S_History.FindAll(StageData.FindScore);
		if (!StageData.OnReplay)
		{
			StageData.C_History.StartTimes++;
		}
		MyPlane.HiScore = StageData.S_History[0].Score;
	}

	public override void UpdateData()
	{
		if (StageData.OnReplay && KClass.Key_minus)
		{
			Slowflag++;
			if (Slowflag < 4)
			{
				return;
			}
			Slowflag = 0;
		}
		base.UpdateData();
		if (StageData.SlowMode > 0)
		{
			StageData.SlowMode--;
			if (TimeMain % 2 == 0)
			{
				return;
			}
		}
		if (KClass.Key_ESC)
		{
			StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "PauseMenu",
				NeedInit = true,
				SDPswitch = new StageDataPackage(GlobalData)
			};
			GlobalData.LastState = this;
			StageData.SoundPlay("se_pause.wav");
		}
		if (StageData.Rep != null)
		{
			if (StageData.OnReplay)
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
			else
			{
				StageData.Rep.WriteKey(StageData.KClass.Key2Hex());
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
		for (int num2 = MyBulletList.Count - 1; num2 >= 0; num2--)
		{
			MyBulletList[num2].Ctrl();
		}
		MyPlane.Ctrl();
		for (int num3 = SpellList.Count - 1; num3 >= 0; num3--)
		{
			SpellList[num3].Ctrl();
		}
		for (int num4 = EnemyPlaneList.Count - 1; num4 >= 0; num4--)
		{
			EnemyPlaneList[num4].Ctrl();
		}
		for (int num5 = BulletList.Count - 1; num5 >= 0; num5--)
		{
			BulletList[num5].Ctrl();
		}
		if (Boss != null)
		{
			Boss.Ctrl();
		}
		for (int num6 = EffectList.Count - 1; num6 >= 0; num6--)
		{
			EffectList[num6].Ctrl();
		}
		Particle3D.Ctrl();
		for (int num7 = ItemList.Count - 1; num7 >= 0; num7--)
		{
			ItemList[num7].Ctrl();
		}
		StageData.Vibrate();
		HitCheckAllinOne();
		for (int num8 = InterfaceList.Count - 1; num8 >= 0; num8--)
		{
			InterfaceList[num8].Ctrl();
		}
		SoundPlayList.ForEach((XAudio2_Player x) =>
		{
			x.Play();
		});
		if (StageData.OnReplay && TimeMain % 6 != 0 && KClass.Key_plus && MyPlane.Life >= 0 && StateSwitchData == null)
		{
			UpdateData();
		}
		GlobalData.LastState = this;
	}

	public virtual void HitCheckAllinOne()
	{
		if (MyPlane.Life < 0)
		{
			StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "GameOverMenu",
				NeedInit = true,
				SDPswitch = new StageDataPackage(GlobalData)
			};
		}
		ItemList.ForEach((BaseItem x) =>
		{
			x.HitCheckAll();
		});
		if (Story == null)
		{
			EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
			{
				x.HitCheckAll();
			});
			BulletList.ForEach((BaseBullet_Touhou x) =>
			{
				x.HitCheckAll();
			});
			if (Boss != null)
			{
				Boss.HitCheckAll();
			}
			SpellList.ForEach((BaseObject x) =>
			{
				x.HitCheckAll();
			});
		}
	}

	public virtual void ShowInterface()
	{
		if (MyPlane != null)
		{
			MyPlane.ShowCenter();
		}
		SpriteMain.Draw2D(TextureObjectDictionary["TotalBackground"], 1f, 0f, new PointF(320f, 240f), 255);
		DrawText(FPS.ToString("F1"), new Point(580, 460));
	}

	public virtual void Drama()
	{
		if (!StageData.OnReplay)
		{
			StageData.C_History.PlayingTime++;
		}
		if (TimeMain == 1)
		{
			if (StageData.StateSwitchData == null)
			{
				StateSwitchData = new StateSwitchDataPackage
				{
					NextState = "MusicLoading",
					NeedInit = true,
					SDPswitch = new StageDataPackage(GlobalData)
				};
			}
			GlobalData.LastState = this;
			StageData.CurrentStageName = StageName;
			RoadFlag = false;
			EffectList.Clear();
			MyPlane.Refresh();
			Ran = new MyRandom(0);
		}
	}

	public override void Render()
	{
		//IL_0031: Unknown result type (might be due to invalid IL or missing references)
		//IL_0040: Unknown result type (might be due to invalid IL or missing references)
		//IL_02ab: Unknown result type (might be due to invalid IL or missing references)
		//IL_02ba: Unknown result type (might be due to invalid IL or missing references)
		//IL_0561: Unknown result type (might be due to invalid IL or missing references)
		//IL_05c2: Unknown result type (might be due to invalid IL or missing references)
		//IL_05d3: Unknown result type (might be due to invalid IL or missing references)
		GlobalData.ScreenTexMan.Begin();
		Background3D.Show();
		SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		Background2.Show(Active: true);
		SpriteMain.End();
		SpriteMain.Begin((SpriteFlags)16);
		TextureObject txtureObject = TextureObjectDictionary["White"];
		switch (MyPlane.EnchantmentState)
		{
		case EnchantmentType.Red:
			SpriteMain.Draw2D(txtureObject, 100f, 0f, new PointF((BoundRect.Left + BoundRect.Right) / 2, (BoundRect.Top + BoundRect.Height) / 2), Color.FromArgb(180, 30, 0, 0));
			break;
		case EnchantmentType.Blue:
			SpriteMain.Draw2D(txtureObject, 100f, 0f, new PointF((BoundRect.Left + BoundRect.Right) / 2, (BoundRect.Top + BoundRect.Height) / 2), Color.FromArgb(180, 0, 0, 30));
			break;
		case EnchantmentType.Green:
			SpriteMain.Draw2D(txtureObject, 100f, 0f, new PointF((BoundRect.Left + BoundRect.Right) / 2, (BoundRect.Top + BoundRect.Height) / 2), Color.FromArgb(180, 0, 30, 0));
			break;
		}
		Background2.Show(Active: false);
		if (Boss != null)
		{
			Boss.Show();
		}
		MyBulletList.ForEach((BaseObject x) =>
		{
			x.Show();
		});
		MyPlane.Show();
		EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
		{
			x.Show();
		});
		ItemList.ForEach((BaseItem x) =>
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
		SpellList.ForEach((BaseObject x) =>
		{
			x.Show();
		});
		ItemList.ForEach((BaseItem x) =>
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
		SpriteMain.End();
		Particle3D.Show();
		SpriteMain.Begin((SpriteFlags)16);
		if (Story != null)
		{
			Story.Show();
		}
		ShowInterface();
		InterfaceList.ForEach((BaseObject x) =>
		{
			x.Show();
		});
		SpriteMain.End();
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

	protected override void Dispose(bool disposing)
	{
		if (disposing && components != null)
		{
			components.Dispose();
		}
		((ContainerControl)this).Dispose(disposing);
	}

	private void InitializeComponent()
	{
		//IL_00ea: Unknown result type (might be due to invalid IL or missing references)
		//IL_017e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0414: Unknown result type (might be due to invalid IL or missing references)
		//IL_06a8: Unknown result type (might be due to invalid IL or missing references)
		//IL_0721: Unknown result type (might be due to invalid IL or missing references)
		gameRegion_ScoreValue = new GameRegion();
		gameRegion_SpellLine = new GameRegion();
		gameRegion_Power = new GameRegion();
		gameRegion_Spell = new GameRegion();
		gameRegion_Player = new GameRegion();
		gameRegion_Score = new GameRegion();
		gameRegion_LifeSelf = new GameRegion();
		GameRegion_Main = new GameRegion();
		gameRegion_BluePoint = new GameRegion();
		gameRegion_GreenPoint = new GameRegion();
		gameRegion_Graze = new GameRegion();
		gameRegion_HiScoreValue = new GameRegion();
		gameRegion1 = new GameRegion();
		gameRegion_HiScore = new GameRegion();
		((Control)this).SuspendLayout();
		((Control)gameRegion_ScoreValue).BackColor = Color.Transparent;
		((Control)gameRegion_ScoreValue).BackgroundImageLayout = (ImageLayout)4;
		((UserControl)gameRegion_ScoreValue).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_ScoreValue).Location = new Point(488, 70);
		((Control)gameRegion_ScoreValue).Margin = new Padding(10, 1, 3, 1);
		((Control)gameRegion_ScoreValue).Name = "gameRegion_ScoreValue";
		((Control)gameRegion_ScoreValue).Size = new Size(110, 16);
		((Control)gameRegion_ScoreValue).TabIndex = 7;
		((Control)gameRegion_SpellLine).BackColor = Color.Transparent;
		((Control)gameRegion_SpellLine).BackgroundImage = (Image)(object)Resources.FiveStar;
		((Control)gameRegion_SpellLine).BackgroundImageLayout = (ImageLayout)4;
		((UserControl)gameRegion_SpellLine).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_SpellLine).Location = new Point(504, 127);
		((Control)gameRegion_SpellLine).Margin = new Padding(10, 1, 3, 1);
		((Control)gameRegion_SpellLine).Name = "gameRegion_SpellLine";
		((Control)gameRegion_SpellLine).Size = new Size(110, 16);
		((Control)gameRegion_SpellLine).TabIndex = 6;
		((Control)gameRegion_Power).BackColor = Color.Transparent;
		((Control)gameRegion_Power).BackgroundImage = (Image)(object)Resources.FiveStar;
		((Control)gameRegion_Power).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_Power).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_Power).Location = new Point(442, 188);
		((Control)gameRegion_Power).Name = "gameRegion_Power";
		((Control)gameRegion_Power).Size = new Size(163, 25);
		((Control)gameRegion_Power).TabIndex = 5;
		((Control)gameRegion_Spell).BackColor = Color.Transparent;
		((Control)gameRegion_Spell).BackgroundImage = (Image)(object)Resources.Spell;
		((Control)gameRegion_Spell).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_Spell).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_Spell).Location = new Point(428, 125);
		((Control)gameRegion_Spell).Name = "gameRegion_Spell";
		((Control)gameRegion_Spell).Size = new Size(70, 16);
		((Control)gameRegion_Spell).TabIndex = 4;
		((Control)gameRegion_Player).BackColor = Color.Transparent;
		((Control)gameRegion_Player).BackgroundImage = (Image)(object)Resources.Player;
		((Control)gameRegion_Player).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_Player).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_Player).Location = new Point(428, 99);
		((Control)gameRegion_Player).Name = "gameRegion_Player";
		((Control)gameRegion_Player).Size = new Size(70, 16);
		((Control)gameRegion_Player).TabIndex = 3;
		((Control)gameRegion_Score).BackColor = Color.Transparent;
		((Control)gameRegion_Score).BackgroundImage = (Image)(object)Resources.Score;
		((Control)gameRegion_Score).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_Score).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_Score).Location = new Point(428, 67);
		((Control)gameRegion_Score).Name = "gameRegion_Score";
		((Control)gameRegion_Score).Size = new Size(70, 16);
		((Control)gameRegion_Score).TabIndex = 2;
		((Control)gameRegion_LifeSelf).BackColor = Color.Transparent;
		((Control)gameRegion_LifeSelf).BackgroundImage = (Image)(object)Resources.MyPlane;
		((Control)gameRegion_LifeSelf).BackgroundImageLayout = (ImageLayout)4;
		((UserControl)gameRegion_LifeSelf).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_LifeSelf).Location = new Point(504, 101);
		((Control)gameRegion_LifeSelf).Margin = new Padding(10, 1, 3, 1);
		((Control)gameRegion_LifeSelf).Name = "gameRegion_LifeSelf";
		((Control)gameRegion_LifeSelf).Size = new Size(110, 16);
		((Control)gameRegion_LifeSelf).TabIndex = 0;
		((Control)GameRegion_Main).Location = new Point(36, 16);
		((Control)GameRegion_Main).MaximumSize = new Size(384, 448);
		((Control)GameRegion_Main).MinimumSize = new Size(384, 448);
		((Control)GameRegion_Main).Name = "GameRegion_Main";
		((Control)GameRegion_Main).Size = new Size(384, 448);
		((Control)GameRegion_Main).TabIndex = 1;
		((Control)gameRegion_BluePoint).BackColor = Color.Transparent;
		((Control)gameRegion_BluePoint).BackgroundImage = (Image)(object)Resources.FiveStar;
		((Control)gameRegion_BluePoint).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_BluePoint).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_BluePoint).Location = new Point(442, 214);
		((Control)gameRegion_BluePoint).Name = "gameRegion_BluePoint";
		((Control)gameRegion_BluePoint).Size = new Size(163, 25);
		((Control)gameRegion_BluePoint).TabIndex = 9;
		((Control)gameRegion_GreenPoint).BackColor = Color.Transparent;
		((Control)gameRegion_GreenPoint).BackgroundImage = (Image)(object)Resources.FiveStar;
		((Control)gameRegion_GreenPoint).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_GreenPoint).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_GreenPoint).Location = new Point(442, 240);
		((Control)gameRegion_GreenPoint).Name = "gameRegion_GreenPoint";
		((Control)gameRegion_GreenPoint).Size = new Size(163, 25);
		((Control)gameRegion_GreenPoint).TabIndex = 10;
		((Control)gameRegion_Graze).BackColor = Color.Transparent;
		((Control)gameRegion_Graze).BackgroundImage = (Image)(object)Resources.FiveStar;
		((Control)gameRegion_Graze).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_Graze).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_Graze).Location = new Point(442, 266);
		((Control)gameRegion_Graze).Name = "gameRegion_Graze";
		((Control)gameRegion_Graze).Size = new Size(163, 25);
		((Control)gameRegion_Graze).TabIndex = 11;
		((Control)gameRegion_HiScoreValue).BackColor = Color.Transparent;
		((Control)gameRegion_HiScoreValue).BackgroundImageLayout = (ImageLayout)4;
		((UserControl)gameRegion_HiScoreValue).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_HiScoreValue).Location = new Point(488, 47);
		((Control)gameRegion_HiScoreValue).Margin = new Padding(10, 1, 3, 1);
		((Control)gameRegion_HiScoreValue).Name = "gameRegion_HiScoreValue";
		((Control)gameRegion_HiScoreValue).Size = new Size(110, 16);
		((Control)gameRegion_HiScoreValue).TabIndex = 12;
		((Control)gameRegion1).BackColor = Color.Transparent;
		((Control)gameRegion1).BackgroundImageLayout = (ImageLayout)4;
		((Control)gameRegion1).Location = new Point(451, 48);
		((Control)gameRegion1).Margin = new Padding(10, 3, 3, 3);
		((Control)gameRegion1).Name = "gameRegion1";
		((Control)gameRegion1).Size = new Size(165, 20);
		((Control)gameRegion1).TabIndex = 12;
		((Control)gameRegion_HiScore).BackColor = Color.Transparent;
		((Control)gameRegion_HiScore).BackgroundImage = (Image)(object)Resources.Score;
		((Control)gameRegion_HiScore).BackgroundImageLayout = (ImageLayout)0;
		((UserControl)gameRegion_HiScore).BorderStyle = (BorderStyle)2;
		((Control)gameRegion_HiScore).Location = new Point(428, 44);
		((Control)gameRegion_HiScore).Name = "gameRegion_HiScore";
		((Control)gameRegion_HiScore).Size = new Size(70, 16);
		((Control)gameRegion_HiScore).TabIndex = 13;
		((ContainerControl)this).AutoScaleMode = (AutoScaleMode)0;
		((Control)this).BackgroundImage = (Image)(object)Resources.background;
		((Control)this).Controls.Add((Control)(object)gameRegion_Score);
		((Control)this).Controls.Add((Control)(object)gameRegion_HiScore);
		((Control)this).Controls.Add((Control)(object)gameRegion_HiScoreValue);
		((Control)this).Controls.Add((Control)(object)gameRegion_Graze);
		((Control)this).Controls.Add((Control)(object)gameRegion_GreenPoint);
		((Control)this).Controls.Add((Control)(object)gameRegion_BluePoint);
		((Control)this).Controls.Add((Control)(object)gameRegion_ScoreValue);
		((Control)this).Controls.Add((Control)(object)gameRegion_SpellLine);
		((Control)this).Controls.Add((Control)(object)gameRegion_Spell);
		((Control)this).Controls.Add((Control)(object)gameRegion_Player);
		((Control)this).Controls.Add((Control)(object)gameRegion_LifeSelf);
		((Control)this).Controls.Add((Control)(object)GameRegion_Main);
		((Control)this).Controls.Add((Control)(object)gameRegion_Power);
		((Control)this).Name = "BaseStage";
		((Control)this).Size = new Size(640, 480);
		((Control)this).ResumeLayout(false);
	}
}
