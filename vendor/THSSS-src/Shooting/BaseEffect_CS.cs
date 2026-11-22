using System.Collections.Generic;

namespace Shooting;

public class BaseEffect_CS : BaseEffect
{
	public BaseEffect_CS()
	{
	}

	public BaseEffect_CS(StageDataPackage StageData)
	{
		base.StageData = StageData;
		EventGroupList = new List<EventGroup>();
		EventsExecutionList = new List<Execution>();
	}

	public override void EventCtrl()
	{
		EventGroupList.ForEach((EventGroup a) =>
		{
			a.Update(this);
		});
		EventsExecutionList.ForEach((Execution a) =>
		{
			a.Update(this);
		});
	}
}
