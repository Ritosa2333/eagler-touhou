using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class SpellCard_SSS05_00 : BaseSpellCard
{
	public SpellCard_SSS05_00(StageDataPackage StageData)
		: base(StageData)
	{
		if (Difficulty < DifficultLevel.Hard)
		{
			SC_Name = "杀箭「新星爆炸箭」";
		}
		else
		{
			SC_Name = "杀箭「新星爆炸箭」";
		}
		BaseScore = 30000000L;
		Boss.DestPoint = new Point(BoundRect.Width / 2, 120);
		Boss.Velocity = 4f;
		StageData.SoundPlay("se_cat00.wav");
		new BackgroundRotate(StageData, "Card02b")
		{
			Scale = 1.3f,
			TransparentValueF = 255f,
			AngularVelocityDegree = -0.5f
		};
		BackgroundMove2 backgroundMove = new BackgroundMove2(StageData, "Card02a")
		{
			DirectionDegree = -80.0,
			TransparentValueF = 155f,
			Scale = 1f,
			Active = false,
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
		new BackgroundRotate(StageData, "Card02c")
		{
			Scale = 0.7f,
			Active = true,
			AngularVelocityDegree = 0.2f
		};
		new BulletTitle(StageData, SC_Name);
		new SpellCardAttack(StageData);
		new FullPic2(StageData, "FaceRakuki_ct");
	}

	public override void Shoot()
	{
		Boss.Armon = Boss.ArmonArray[1];
		if (Time > 100)
		{
			Boss.MoveUpDown();
		}
		if (Time == 150)
		{
			string fileName = (int)Difficulty switch
			{
				0 => ".\\CS\\St05\\道中Boss\\1符E.mbg", 
				1 => ".\\CS\\St05\\道中Boss\\1符N.mbg", 
				2 => ".\\CS\\St05\\道中Boss\\1符H.mbg", 
				3 => ".\\CS\\St05\\道中Boss\\1符L.mbg", 
				_ => ".\\CS\\St05\\道中Boss\\1符L.mbg", 
			};
			CS_Data cSData = StageData.LoadCS(fileName);
			new CSEmitterController(StageData, cSData);
		}
	}
}
