using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS06_06 : BaseSpellCard
{
	private int flag = 1;

	public SpellCard_SSS06_06(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "「流星神话」";
		}
		else
		{
			SC_Name = "「流星神话」";
		}
		BaseScore = 35000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundBoss06(StageData);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic3(StageData, "FaceTensei_ct3");
	}

	public override void Shoot()
	{
		if (SpellList.Count > 0 || MyPlane.Time < MyPlane.UnmatchedTime - 30)
		{
			Boss.Armon = 1f;
		}
		else
		{
			Boss.Armon = Boss.ArmonArray[9];
		}
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\6符P1E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\6符P1N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\6符P1H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\6符P1L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\6符P1L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
		if ((Boss.HealthPoint < (float)(Boss.MaxHP * 3) / 4f || Time > 1800) && flag == 1)
		{
			flag++;
			ClearEmitter();
			string fileName2 = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\6符P2E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\6符P2N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\6符P2H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\6符P2L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\6符P2L.mbg", 
			};
			CS_Data cSData2 = StageData.LoadCS(fileName2);
			new CSEmitterController(StageData, cSData2);
		}
		if ((Boss.HealthPoint < (float)(Boss.MaxHP * 2) / 4f || Time > 3600) && flag == 2)
		{
			flag++;
			ClearEmitter();
			string fileName3 = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\6符P3E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\6符P3N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\6符P3H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\6符P3L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\6符P2L.mbg", 
			};
			CS_Data cSData3 = StageData.LoadCS(fileName3);
			new CSEmitterController(StageData, cSData3);
		}
		if ((Boss.HealthPoint < (float)Boss.MaxHP / 4f || Time > 5400) && flag == 3)
		{
			flag++;
			ClearEmitter();
			string fileName4 = (int)Difficulty switch
			{
				0 => ".\\CS\\St06\\关底Boss\\6符P4E.mbg", 
				1 => ".\\CS\\St06\\关底Boss\\6符P4N.mbg", 
				2 => ".\\CS\\St06\\关底Boss\\6符P4H.mbg", 
				3 => ".\\CS\\St06\\关底Boss\\6符P4L.mbg", 
				_ => ".\\CS\\St06\\关底Boss\\6符P4L.mbg", 
			};
			CS_Data cSData4 = StageData.LoadCS(fileName4);
			new CSEmitterController(StageData, cSData4);
		}
	}

	private void ClearEmitter()
	{
		for (int num = EnemyPlaneList.Count - 1; num >= 0; num--)
		{
			if (!(EnemyPlaneList[num] is BaseSpellCard))
			{
				EnemyPlaneList[num].GiveEndEffect();
				EnemyPlaneList.RemoveAt(num);
			}
		}
		foreach (BaseBullet_Touhou bullet in BulletList)
		{
			bullet.GiveEndEffect();
		}
		BulletList.Clear();
	}
}
