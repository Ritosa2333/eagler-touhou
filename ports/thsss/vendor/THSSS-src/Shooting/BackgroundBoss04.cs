using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class BackgroundBoss04 : BaseObject
{
	public BackgroundBoss04(StageDataPackage StageData)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		new BackgroundSC(StageData, "Card04c");
		BackgroundRotate backgroundRotate = new BackgroundRotate(StageData, "Card04a")
		{
			DirectionDegree = 90.0,
			TransparentValueF = 128f,
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
		obj.EventString = "当前帧=1：不透明度变化到5，正弦，200";
		obj.String2BulletEvent();
		eventGroup.EventList.Add(obj);
		backgroundRotate.EventGroupList.Add(eventGroup);
		BackgroundRotate backgroundRotate2 = new BackgroundRotate(StageData, "Card04b")
		{
			DirectionDegree = 90.0,
			TransparentValueF = 128f,
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
		obj.EventString = "当前帧=1：不透明度变化到255，正弦，200";
		obj.String2BulletEvent();
		eventGroup.EventList.Add(obj);
		backgroundRotate2.EventGroupList.Add(eventGroup);
	}
}
