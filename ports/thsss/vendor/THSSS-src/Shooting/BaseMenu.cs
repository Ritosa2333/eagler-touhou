using System.Windows.Forms;

namespace Shooting;

internal class BaseMenu : BaseGameState, IGameState
{
	public int BackgroundLight { get; set; }

	public BaseMenu(GlobalDataPackage GlobalData)
	{
		((Control)this).Visible = false;
		((Control)this).Enabled = false;
		base.GlobalData = GlobalData;
	}

	public BaseMenu()
	{
	}

	public override void Init()
	{
		MenuInit();
	}

	public virtual void MenuInit()
	{
	}

	public override void UpdateData()
	{
		base.UpdateData();
		int count = StageData.MenuGroupList.Count;
		if (count > 0)
		{
			StageData.MenuGroupList[count - 1].MenuSelect();
		}
		StageData.MenuGroupList.ForEach((BaseMenuGroup x) =>
		{
			x.Ctrl();
		});
		Background3D.Ctrl();
		Background.Ctrl();
		Background2.Ctrl();
		for (int num = EffectList.Count - 1; num >= 0; num--)
		{
			EffectList[num].Ctrl();
		}
		Particle3D.Ctrl();
		SoundPlayList.ForEach((XAudio2_Player x) =>
		{
			x.Play();
		});
	}

	public virtual void BackgroundDrama()
	{
	}

	private void InitializeComponent()
	{
		((Control)this).SuspendLayout();
		((ContainerControl)this).AutoScaleMode = (AutoScaleMode)0;
		((Control)this).Name = "BaseMenu";
		((Control)this).ResumeLayout(false);
	}
}
