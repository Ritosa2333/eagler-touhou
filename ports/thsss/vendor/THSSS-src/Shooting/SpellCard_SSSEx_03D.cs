using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_03D : BaseSpellCard
{
	public SpellCard_SSSEx_03D(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "倒映「天之月水之月」";
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
		Boss.Armon = Boss.ArmonArray[5];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\3符3.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
