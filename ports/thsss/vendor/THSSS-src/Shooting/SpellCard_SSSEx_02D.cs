using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_02D : BaseSpellCard
{
	public SpellCard_SSSEx_02D(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "幽光「幻月蝶」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRika_ct3");
		new BackgroundBossEx(StageData);
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[3];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\2符3.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
