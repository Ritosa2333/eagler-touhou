using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class GameState_testStage : BaseStage, IGameState
{
	public GameState_testStage(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Stage_test";
	}

	public override void Drama()
	{
		if (TimeMain == 1)
		{
			StageData.SetReplayInfo(StageName);
			if (MyPlane == null)
			{
				MyPlane = new BaseMyPlane(StageData, new Point(BoundRect.Width, BoundRect.Height - 50));
			}
			new BackgroundRotate(StageData, "Card02b")
			{
				Scale = 1.3f,
				TransparentValueF = 255f,
				AngularVelocityDegree = -0.5f
			};
			BackgroundRotate backgroundRotate = new BackgroundRotate(StageData, "Card02a")
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
			new BackgroundRotate(StageData, "Card02c")
			{
				Scale = 0.7f,
				Active = true,
				AngularVelocityDegree = 0.2f
			};
		}
		_ = TimeMain;
		_ = 50;
		_ = TimeMain % 400;
		_ = 100;
	}

	public override void BGM_ON()
	{
	}
}
