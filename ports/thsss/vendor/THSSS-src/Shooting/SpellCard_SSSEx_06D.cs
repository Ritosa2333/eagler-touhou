using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_06D : BaseSpellCard
{
	public SpellCard_SSSEx_06D(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "皎符「星月炮」";
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
		Boss.Armon = Boss.ArmonArray[11];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\6符3.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
