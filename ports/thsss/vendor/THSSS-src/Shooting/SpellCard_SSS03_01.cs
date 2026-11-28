using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS03_01 : BaseSpellCard
{
	public SpellCard_SSS03_01(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "火符「逼近的火焰」";
		}
		else
		{
			SC_Name = "火符「燃烧地狱火海」";
		}
		BaseScore = 20000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundRotate(StageData, "Card03b")
		{
			Scale = 1.3f,
			TransparentValueF = 255f,
			AngularVelocityDegree = -0.5f
		};
		BackgroundRotate backgroundRotate = new BackgroundRotate(StageData, "Card03a")
		{
			DirectionDegree = 90.0,
			TransparentValueF = 155f,
			Scale = 1f,
			Active = false,
			AngularVelocityDegree = 0f,
			EventGroupList = new List<EventGroup>(),
			EventsExecutionList = new List<Execution>()
		};
		EventGroup eventGroup = new EventGroup
		{
			index = 0,
			tag = "0",
			t = 200,
			addtime = 200
		};
		Event obj = new Event();
		obj.EventString = "当前帧=1：不透明度变化到60，正弦，400";
		obj.String2BulletEvent();
		eventGroup.EventList.Add(obj);
		backgroundRotate.EventGroupList.Add(eventGroup);
		new BackgroundRotate(StageData, "Card03c")
		{
			Scale = 0.7f,
			Active = true,
			AngularVelocityDegree = 0.2f
		};
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceKoreirei_ct").Scale = 0.8f;
	}

	public override void Shoot()
	{
		Boss.Armon = 0.15f;
		if (Time > 50)
		{
			Boss.RandomMove(150, 20, 2f, new Rectangle(BoundRect.Width / 2 - 45, 100, 90, 40));
		}
		if (Time == 100)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St03\\道中Boss\\1符E.mbg", 
				1 => ".\\CS\\St03\\道中Boss\\1符N.mbg", 
				2 => ".\\CS\\St03\\道中Boss\\1符H.mbg", 
				3 => ".\\CS\\St03\\道中Boss\\1符L.mbg", 
				_ => ".\\CS\\St03\\道中Boss\\1符U.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData).BossBinding = true;
		}
	}
}
