using System;
using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

public class BaseObject_CS : BaseObject
{
	public int type;

	public virtual int Type
	{
		get
		{
			return type;
		}
		set
		{
			type = value;
		}
	}

	public bool Ghosting
	{
		get
		{
			return GhostingCount > 0;
		}
		set
		{
			if (value)
			{
				GhostingCount = 20;
				GhostingColor = Color.White;
			}
			else
			{
				GhostingCount = 0;
			}
		}
	}

	public bool OutBound
	{
		get
		{
			return OutsideRegion < 80;
		}
		set
		{
			if (!value)
			{
				OutsideRegion = 200;
			}
		}
	}

	public bool BeginningEffect { get; set; }

	public bool EndingEffect { get; set; }

	public PointF CS_Position
	{
		get
		{
			return new PointF(OriginalPosition.X + 320f - 192f, OriginalPosition.Y + 240f - 224f);
		}
		set
		{
			OriginalPosition = new PointF(value.X - 320f + 192f, value.Y - 240f + 224f);
		}
	}

	public int ID { get; set; }

	public int LayerID { get; set; }

	public float AccelerateCS { get; set; }

	public double AccDirection { get; set; }

	public double RanAngle { get; set; }

	public float RanVelocity { get; set; }

	public double RanDirection { get; set; }

	public float RanAccelerate { get; set; }

	public double RanAccDirection { get; set; }

	public bool Cover { get; set; }

	public bool Rebound { get; set; }

	public bool Force { get; set; }

	public List<EventGroup> EventGroupList { get; set; }

	public List<Execution> EventsExecutionList { get; set; }

	public BaseObject_CS()
	{
	}

	public BaseObject_CS(StageDataPackage StageData)
		: base(StageData)
	{
	}

	public BaseObject_CS(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
	}

	public override void Ctrl()
	{
		double num = AccDirection / 180.0 * Math.PI;
		Ax = AccelerateCS * (float)Math.Cos(num);
		Ay = AccelerateCS * (float)Math.Sin(num);
		EventCtrl();
		base.Ctrl();
	}

	public virtual void EventCtrl()
	{
		if (EventGroupList != null)
		{
			EventGroupList.ForEach((EventGroup a) =>
			{
				a.Update(this);
			});
		}
		if (EventsExecutionList != null)
		{
			EventsExecutionList.ForEach((Execution a) =>
			{
				a.Update(this);
			});
		}
	}

	public override object Clone()
	{
		BaseObject_CS b = (BaseObject_CS)base.Clone();
		b.EventsExecutionList = new List<Execution>();
		b.EventGroupList = new List<EventGroup>();
		EventGroupList.ForEach((EventGroup x) =>
		{
			b.EventGroupList.Add((EventGroup)x.Clone());
		});
		return b;
	}
}
