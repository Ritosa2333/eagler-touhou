using System.Drawing;

namespace Shooting;

internal class EndBoss_FSC06 : EndBoss_Touhou_Small
{
	private int tempVolumn;

	public EndBoss_FSC06(StageDataPackage StageData, PointF OriginalPosition, Color Color1, Color Color2)
		: base(StageData, OriginalPosition, Color1, Color2)
	{
		tempVolumn = BGM_Player.Volume;
		LifeTime = 300;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		int num = LifeTime - 90;
		BGM_Player.Volume = (int)((float)tempVolumn * (1f - (float)Time / (float)num));
		PointF originalPosition = new PointF(BoundRect.Width / 2, 150f);
		if (Time == LifeTime - 60 || Time == LifeTime - 40 || Time == LifeTime - 20)
		{
			new EmitterSaveEnegy3D(StageData, originalPosition, Color.SkyBlue);
			StageData.SoundPlay("se_ch02.wav");
		}
		if (Time == LifeTime)
		{
			StageData.ChangeBGM(".\\BGM\\Boss06FSC.wav", 0, 0, 255, 0, 0);
			new Boss_Tensei02(StageData);
			Background3D.WarpEnabled = true;
		}
	}
}
