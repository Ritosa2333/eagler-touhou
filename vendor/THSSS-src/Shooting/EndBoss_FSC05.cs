using System.Drawing;

namespace Shooting;

internal class EndBoss_FSC05 : EndBoss_Touhou_Small
{
	public EndBoss_FSC05(StageDataPackage StageData, PointF OriginalPosition, Color Color1, Color Color2)
		: base(StageData, OriginalPosition, Color1, Color2)
	{
		LifeTime = 150;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		PointF originalPosition = new PointF(BoundRect.Width / 2, 150f);
		if (Time == LifeTime - 60 || Time == LifeTime - 40 || Time == LifeTime - 20)
		{
			new EmitterSaveEnegy3D(StageData, originalPosition, Color.Red);
			StageData.SoundPlay("se_ch02.wav");
		}
		if (Time == LifeTime)
		{
			new Boss_Rakukun02(StageData);
			Background3D.WarpEnabled = true;
		}
	}
}
