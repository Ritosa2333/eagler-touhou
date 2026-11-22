using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class BackgroundBoss05 : BaseObject
{
	public BackgroundBoss05(StageDataPackage StageData)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		new BackgroundRotate(StageData, "card05b")
		{
			Scale = 1.3f,
			TransparentValueF = 255f,
			AngularVelocityDegree = -0.5f
		};
		BackgroundMove2 backgroundMove = new BackgroundMove2(StageData, "card05c")
		{
			DirectionDegree = -180.0,
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
		new BackgroundRotate(StageData, "card05a")
		{
			Scale = 0.7f,
			Active = true,
			AngularVelocityDegree = 0.2f
		};
	}
}
