using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS03_05 : BaseSpellCard
{
	public SpellCard_SSS03_05(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "绿符「柳叶风暴」";
		}
		else
		{
			SC_Name = "深绿「舞动的翠绿之风」";
		}
		BaseScore = 20000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundRotate(StageData, "Card03f")
		{
			Scale = 1f,
			AngularVelocityDegree = 0f,
			AngleDegree = 90.0,
			AngleWithDirection = false
		};
		BackgroundMove2 backgroundMove = new BackgroundMove2(StageData, "Card03d")
		{
			Velocity = 1f,
			DirectionDegree = 0.0,
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
		backgroundMove.EventGroupList.Add(eventGroup);
		BackgroundMove2 backgroundMove2 = new BackgroundMove2(StageData, "Card03e")
		{
			Velocity = 1f,
			DirectionDegree = 180.0,
			TransparentValueF = 60f,
			Scale = 1f,
			Active = false,
			AngularVelocityDegree = 0f,
			EventGroupList = new List<EventGroup>(),
			EventsExecutionList = new List<Execution>()
		};
		eventGroup = new EventGroup
		{
			index = 0,
			tag = "0",
			t = 200,
			addtime = 200
		};
		obj = new Event();
		obj.EventString = "当前帧=1：不透明度变化到155，正弦，400";
		obj.String2BulletEvent();
		eventGroup.EventList.Add(obj);
		backgroundMove2.EventGroupList.Add(eventGroup);
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceSeiryuu_ct");
	}

	public override void Shoot()
	{
		Boss.Armon = 0.9f;
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St03\\关底Boss\\3符E.mbg", 
				1 => ".\\CS\\St03\\关底Boss\\3符N.mbg", 
				2 => ".\\CS\\St03\\关底Boss\\3符H.mbg", 
				3 => ".\\CS\\St03\\关底Boss\\3符L.mbg", 
				_ => ".\\CS\\St03\\关底Boss\\3符U.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
