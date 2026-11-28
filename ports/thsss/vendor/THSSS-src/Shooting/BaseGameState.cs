using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BaseGameState : UserControl, IGameState
{
	public GlobalDataPackage GlobalData { get; set; }

	public StageDataPackage StageData { get; set; }

	public string StageName { get; set; }

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

	public MyRandom Ran
	{
		get
		{
			return StageData.Ran;
		}
		set
		{
			StageData.Ran = value;
		}
	}

	public MySprite SpriteMain => StageData.SpriteMain;

	public Rectangle BoundRect
	{
		get
		{
			return StageData.BoundRect;
		}
		set
		{
			StageData.BoundRect = value;
		}
	}

	public BaseMyPlane MyPlane
	{
		get
		{
			return StageData.MyPlane;
		}
		set
		{
			StageData.MyPlane = value;
		}
	}

	public List<BaseBullet_Touhou> BulletList
	{
		get
		{
			return StageData.BulletList;
		}
		set
		{
			StageData.BulletList = value;
		}
	}

	public List<BaseObject> MyBulletList
	{
		get
		{
			return StageData.MyBulletList;
		}
		set
		{
			StageData.MyBulletList = value;
		}
	}

	public List<BaseObject> SpellList
	{
		get
		{
			return StageData.SpellList;
		}
		set
		{
			StageData.SpellList = value;
		}
	}

	public List<BaseEnemyPlane> EnemyPlaneList
	{
		get
		{
			return StageData.EnemyPlaneList;
		}
		set
		{
			StageData.EnemyPlaneList = value;
		}
	}

	public List<BaseEffect> EffectList
	{
		get
		{
			return StageData.EffectList;
		}
		set
		{
			StageData.EffectList = value;
		}
	}

	public List<BaseItem> ItemList
	{
		get
		{
			return StageData.ItemList;
		}
		set
		{
			StageData.ItemList = value;
		}
	}

	public List<BaseObject> InterfaceList
	{
		get
		{
			return StageData.InterfaceList;
		}
		set
		{
			StageData.InterfaceList = value;
		}
	}

	public BaseStory Story
	{
		get
		{
			return StageData.Story;
		}
		set
		{
			StageData.Story = value;
		}
	}

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

	public BackgroundManager Background2
	{
		get
		{
			return StageData.Background2;
		}
		set
		{
			StageData.Background2 = value;
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

	public BaseBossTouhou Boss
	{
		get
		{
			return StageData.Boss;
		}
		set
		{
			StageData.Boss = value;
		}
	}

	public List<XAudio2_Player> SoundPlayList
	{
		get
		{
			return StageData.SoundPlayList;
		}
		set
		{
			StageData.SoundPlayList = value;
		}
	}

	public Dictionary<string, TextureObject> TextureObjectDictionary => GlobalData.TextureObjectDictionary;

	public Dictionary<string, XAudio2_Player> SoundDictionary => GlobalData.SoundDictionary;

	public Wave_Player BGM_Player => GlobalData.BGM_Player;

	public KeyClass KClass => GlobalData.KClass;

	public Device DeviceMain => GlobalData.DeviceMain;

	public double FPS { get; set; }

	public int TimeMain
	{
		get
		{
			return StageData.TimeMain;
		}
		set
		{
			StageData.TimeMain = value;
		}
	}

	private long TimeCount { get; set; }

	public Font DXFont { get; set; }

	public BaseGameState()
	{
	}

	public BaseGameState(GlobalDataPackage GlobalData)
	{
		this.GlobalData = GlobalData;
	}

	public virtual void Init()
	{
	}

	public virtual void UpdateData()
	{
		if (TimeMain % 30 == 0)
		{
			long timestamp = Stopwatch.GetTimestamp();
			FPS = 30f * (float)Stopwatch.Frequency / (float)(timestamp - TimeCount);
			if (TimeMain != 0)
			{
				StageData.RealTimeTotal += timestamp - TimeCount;
				StageData.TimeTotal += 30L;
			}
			TimeCount = timestamp;
		}
		TimeMain++;
		SoundPlayList.Clear();
	}

	public void TimeSycn()
	{
		TimeCount = Stopwatch.GetTimestamp();
	}

	public virtual void Render()
	{
	}

	public void DrawText(string text, Point Pos)
	{
		int num = 0;
		char[] array = text.ToCharArray();
		for (int i = 0; i < array.Length; i++)
		{
			PointF position = new PointF(Pos.X + 5 + num, Pos.Y + 9);
			SpriteMain.Draw2D(TextureObjectDictionary["ScoreNum_" + array[i]], 1f, 0f, position, Color.White);
			num += 16;
			if (array[i] == '.')
			{
				num -= 6;
			}
		}
	}

	public virtual void BGM_ON()
	{
	}

	public virtual void BGM_Resume()
	{
		GlobalData.BGM_Resume();
	}

	public virtual void BGM_Pause()
	{
		GlobalData.BGM_Pause();
	}

	public void Dispose()
	{
		if (StageData != null)
		{
			StageData.Dispose();
		}
		if (DXFont != null)
		{
			((ComObject)DXFont).Dispose();
		}
	}
}
