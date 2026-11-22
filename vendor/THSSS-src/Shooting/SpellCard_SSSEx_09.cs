using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_09 : BaseTimeSC
{
	public SpellCard_SSSEx_09(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "仙蝶「无限的彩灵」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, -150);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new BackgroundBossEx(StageData);
		switch (MyPlane.Name)
		{
		case "Reimu":
			new FullPic2(StageData, "FaceRika_ct0");
			break;
		case "Marisa":
			new FullPic2(StageData, "FaceRika_ct1");
			break;
		case "Sanae":
			new FullPic2(StageData, "FaceRika_ct2");
			break;
		case "Koishi":
			new FullPic2(StageData, "FaceRika_ct3");
			break;
		default:
			new FullPic2(StageData, "FaceRika_ct0");
			break;
		}
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[16];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\9符.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
