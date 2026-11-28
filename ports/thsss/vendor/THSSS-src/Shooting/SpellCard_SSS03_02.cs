using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS03_02 : BaseSpellCard
{
	public SpellCard_SSS03_02(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "火光「火精灵之灾」";
		}
		else
		{
			SC_Name = "火光「午夜的大火灾」";
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
		Boss.Armon = 0.25f;
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			Boss.OnAction = 10000;
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St03\\道中Boss\\2符E.mbg", 
				1 => ".\\CS\\St03\\道中Boss\\2符N.mbg", 
				2 => ".\\CS\\St03\\道中Boss\\2符H.mbg", 
				3 => ".\\CS\\St03\\道中Boss\\2符L.mbg", 
				_ => ".\\CS\\St03\\道中Boss\\2符U.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
