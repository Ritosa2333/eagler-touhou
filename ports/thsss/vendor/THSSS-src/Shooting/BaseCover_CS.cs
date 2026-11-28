using System;
using System.Collections.Generic;

namespace Shooting;

[Serializable]
public class BaseCover_CS : BaseEmitter_CS
{
	public bool Roundness { get; set; }

	public int CtrlID { get; set; }

	public List<EventGroup> SubEventGroupList { get; set; }

	public BaseCover_CS(StageDataPackage StageData)
	{
		base.StageData = StageData;
		EventGroupList = new List<EventGroup>();
		EventsExecutionList = new List<Execution>();
		SubEventGroupList = new List<EventGroup>();
		HealthPoint = 1000000f;
		HitEnabled = false;
		Region = 1;
		Angle = Math.PI / 2.0;
	}

	public override void Shoot()
	{
		AngleWithDirection = Roundness;
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

	public override void HitCheckAll()
	{
		if (Time < StartTime || Time > StartTime + Duration)
		{
			return;
		}
		BulletList.ForEach((BaseBullet_Touhou b) =>
		{
			if (b.Cover && (Type == 0 || b.ID == CtrlID) && HitCheck(b))
			{
				SubEventGroupList.ForEach((EventGroup e) =>
				{
					e.Update(b);
				});
			}
		});
	}

	public override bool HitCheck(BaseObject Target)
	{
		if (Roundness)
		{
			if (ScaleLength == ScaleWidth)
			{
				if (GetDistance(Target) - (double)Target.Region < (double)((float)Region * Scale))
				{
					return true;
				}
			}
			else
			{
				double num = (AngleWithDirection ? (Direction + Angle) : Angle);
				double distance = GetDistance(Target);
				double num2 = GetDirection(Target);
				double num3 = distance * Math.Cos((float)(num - num2));
				double num4 = distance * Math.Sin((float)(num - num2));
				double num5 = (float)Region * ScaleWidth;
				double num6 = (float)Region * ScaleLength;
				if (num4 * num4 / num5 / num5 + num3 * num3 / num6 / num6 < 1.0)
				{
					return true;
				}
			}
			return false;
		}
		if (Math.Abs(Target.OriginalPosition.X - OriginalPosition.X) < (float)Region * ScaleWidth && Math.Abs(Target.OriginalPosition.Y - OriginalPosition.Y) < (float)Region * ScaleLength)
		{
			return true;
		}
		return false;
	}
}
