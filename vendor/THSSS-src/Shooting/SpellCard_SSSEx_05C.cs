using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_05C : BaseSpellCard
{
	public SpellCard_SSSEx_05C(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "六花「冰结界的雪花阵」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRika_ct2");
		new BackgroundBossEx(StageData);
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[9];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\5符2.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
