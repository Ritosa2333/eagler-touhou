using System;
using System.Drawing;

namespace Shooting;

internal class EndBoss : BaseEffect
{
	public EndBoss(StageDataPackage StageData)
		: base(StageData)
	{
		LifeTime = 200;
	}

	public override void Move()
	{
		if (Boss != null)
		{
			Position = Boss.Position;
		}
	}

	public override void Shoot()
	{
		if (Time == 1)
		{
			StageData.SoundPlay("se_enep01.wav");
			for (int i = 0; i < 16; i++)
			{
				new ParticleSmaller(StageData, "光点", Position, 2f + (float)Ran.Next(30) / 10f, (double)((float)Ran.Next(360) / 180f) * Math.PI)
				{
					Scale = (float)Ran.Next(10, 20) / 10f,
					ColorValue = Color.FromArgb(255, Ran.Next(20, 240), Ran.Next(200, 255))
				};
			}
			StageData.VibrateStart(50);
		}
		else if (Time == 100)
		{
			StageData.SoundPlay("se_enep01.wav");
			for (int j = 0; j < 60; j++)
			{
				new ParticleSmaller(StageData, "光点", Position, 1f + (float)Ran.Next(50) / 10f, (double)((float)Ran.Next(360) / 180f) * Math.PI)
				{
					Scale = (float)Ran.Next(10, 20) / 10f,
					ColorValue = Color.FromArgb(255, Ran.Next(20, 240), Ran.Next(220, 255))
				};
			}
			StageData.VibrateStart(50);
			MyBulletList.Clear();
			Boss = null;
		}
		else
		{
			if (Time != 250)
			{
				return;
			}
			new EndStage(StageData, "在世界的某个角落", NextInit: true);
			new TransitionOut(StageData);
			try
			{
				INI_RW iNI_RW = new INI_RW(".\\Setting.INI");
				if (iNI_RW.ExistINIFile())
				{
					iNI_RW.IniWriteValue("Mode", "Clear", "yes");
				}
				iNI_RW = null;
			}
			catch
			{
			}
		}
	}

	public override void Show()
	{
	}
}
