using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_01C : BaseSpellCard
{
	public SpellCard_SSSEx_01C(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "冬临「初雪之寒径」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 224);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRika_ct2");
		new BackgroundBossEx(StageData);
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[1];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\1符2.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
