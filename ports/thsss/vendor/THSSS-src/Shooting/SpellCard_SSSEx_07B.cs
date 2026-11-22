using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_07B : BaseSpellCard
{
	public SpellCard_SSSEx_07B(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "「雾隐流剑技 ～ 血染之樱」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 96);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRika_ct1");
		new BackgroundBossEx(StageData);
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[13];
		if (Time > 100 && (Time - 30) % 780 == 50)
		{
			Boss.DestPoint = new PointF(MyPlane.OriginalPosition.X, Boss.OriginalPosition.Y);
			Boss.Velocity = 4f;
		}
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\7符1.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
