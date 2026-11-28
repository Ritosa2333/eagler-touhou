using System.Drawing;

namespace Shooting;

internal class SpellCard_SSSEx_FSC : BaseSpellCard
{
	public SpellCard_SSSEx_FSC(StageDataPackage StageData)
		: base(StageData)
	{
		SC_Name = "「雾隐之蝶」";
		BaseScore = 40000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
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
		Boss.Armon = Boss.ArmonArray[18];
		if (Time == 100)
		{
			string fileName = ".\\CS\\StEx\\关底Boss\\FSC符.mbg";
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
