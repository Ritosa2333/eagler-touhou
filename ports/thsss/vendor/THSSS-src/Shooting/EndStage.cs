namespace Shooting;

internal class EndStage : BaseEffect
{
	private int tempVolumn;

	private string nextState;

	private bool nextInit;

	public int FadeOutTime { get; set; }

	public EndStage(StageDataPackage StageData, string NextState, bool NextInit)
		: this(StageData, NextState, NextInit, 50)
	{
	}

	public EndStage(StageDataPackage StageData, string NextState, bool NextInit, int FadeOutTime)
		: base(StageData)
	{
		tempVolumn = BGM_Player.Volume;
		LifeTime = 160;
		nextState = NextState;
		nextInit = NextInit;
		this.FadeOutTime = FadeOutTime;
		MyPlane.SpellEnabled = false;
		if (MyPlane.EnchantmentTime > 50)
		{
			MyPlane.EnchantmentTime = 50;
		}
		new BackgroundTransitionOut(StageData, FadeOutTime);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		int num = LifeTime - 100;
		BGM_Player.Volume = (int)((float)tempVolumn * (1f - (float)Time / (float)num));
		if (Time == num)
		{
			StageData.ChangeBGM(BGM_Player.FileName, Repeat: false);
			BGM_Player.Stop();
		}
		if (Time != LifeTime)
		{
			return;
		}
		BGM_Player.Volume = tempVolumn;
		if (nextInit)
		{
			if (StageData.OnReplay && nextState == "MainMenu")
			{
				StageData.Rep.CloseRpy();
				StageData.OnReplay = false;
			}
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = nextState,
				NeedInit = true,
				SDPswitch = new StageDataPackage(StageData.GlobalData)
			};
		}
		else if (StageData.OnPractice)
		{
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "GameSetMenu",
				NeedInit = true,
				SDPswitch = new StageDataPackage(StageData.GlobalData)
			};
		}
		else
		{
			StageData.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = nextState,
				NeedInit = false,
				SDPswitch = StageData
			};
			MyPlane.Refresh();
			StageData.SetReplayStageInfo();
		}
		Background3D.Clear();
		Background.Clear();
		Background2.Clear();
		new BackgroundTransitionOut(StageData, 2)
		{
			TransparentValueF = 255f,
			TransparentVelocity = 0f,
			LifeTime = 1
		};
		BulletList.Clear();
		EnemyPlaneList.Clear();
		ItemList.Clear();
		TimeMain = 0;
		MyPlane.Refresh();
		Ran = new MyRandom(0);
		StageData.VibrateStop();
	}

	public override void Show()
	{
	}
}
