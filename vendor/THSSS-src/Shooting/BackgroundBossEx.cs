using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class BackgroundBossEx : BaseObject
{
	public BackgroundBossEx(StageDataPackage StageData)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		new BackgroundMove2(StageData, "底面").DirectionDegree = 90.0;
		new BackgroundSC(StageData, "中层");
		BackgroundRotate backgroundRotate = new BackgroundRotate(StageData, "最上")
		{
			DirectionDegree = 90.0,
			TransparentValueF = 128f,
			Scale = 1f,
			Active = false,
			AngularVelocityDegree = 0.5f,
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
	}
}
