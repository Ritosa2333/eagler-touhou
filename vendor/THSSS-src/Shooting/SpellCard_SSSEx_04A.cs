using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_04A : BaseSpellCard
{
	public SpellCard_SSSEx_04A(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "舞风「阵风螺旋」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRika_ct0");
		new BackgroundBossEx(StageData);
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[7];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\4符0.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
