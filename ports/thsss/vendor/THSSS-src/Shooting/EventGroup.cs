using System;
using System.Collections.Generic;

namespace Shooting;

[Serializable]
public class EventGroup
{
	public int index;

	public string tag = "";

	public int t = 1;

	public int loop;

	public int addtime;

	public int special;

	public List<Event> EventList = new List<Event>();

	public void Update(BaseEmitter_CS CS_Emitter)
	{
		if (CS_Emitter.Time < 1)
		{
			CS_Emitter.Time = 1;
		}
		float[] array = new float[13];
		float[] array2 = new float[33];
		array[0] = CS_Emitter.Time - CS_Emitter.StartTime + 1;
		array[1] = CS_Emitter.CS_Position.X;
		array[2] = CS_Emitter.CS_Position.Y;
		array[3] = CS_Emitter.EmitRadius;
		array[4] = (float)CS_Emitter.RadiusDirection;
		array[5] = CS_Emitter.Way;
		array[6] = CS_Emitter.Circle;
		array[7] = (float)CS_Emitter.EmitDirection;
		array[8] = (float)CS_Emitter.Range;
		array[9] = CS_Emitter.SubBullet.ScaleWidth;
		array[10] = CS_Emitter.SubBullet.ScaleLength;
		array[11] = CS_Emitter.SubBullet.TransparentValueF;
		array[12] = (float)CS_Emitter.SubBullet.AngleDegree;
		array2[0] = CS_Emitter.CS_Position.X;
		array2[1] = CS_Emitter.CS_Position.Y;
		array2[2] = CS_Emitter.EmitRadius;
		array2[3] = (float)CS_Emitter.RadiusDirection;
		array2[4] = CS_Emitter.Way;
		array2[5] = CS_Emitter.Circle;
		array2[6] = (float)CS_Emitter.EmitDirection;
		array2[7] = (float)CS_Emitter.Range;
		array2[8] = CS_Emitter.Velocity;
		array2[9] = (float)CS_Emitter.DirectionDegree;
		array2[10] = CS_Emitter.AccelerateCS;
		array2[11] = (float)CS_Emitter.AccDirection;
		array2[12] = CS_Emitter.SubBullet.LifeTime;
		array2[13] = CS_Emitter.SubBullet.Type;
		array2[14] = CS_Emitter.SubBullet.ScaleLength;
		array2[15] = CS_Emitter.SubBullet.ScaleLength;
		array2[16] = (int)CS_Emitter.SubBullet.ColorValue.R;
		array2[17] = (int)CS_Emitter.SubBullet.ColorValue.G;
		array2[18] = (int)CS_Emitter.SubBullet.ColorValue.B;
		array2[19] = CS_Emitter.SubBullet.TransparentValueF;
		array2[20] = (float)CS_Emitter.SubBullet.AngleDegree;
		array2[21] = CS_Emitter.SubBullet.Velocity;
		array2[22] = (float)CS_Emitter.SubBullet.DirectionDegree;
		array2[23] = CS_Emitter.SubBullet.AccelerateCS;
		array2[24] = (float)CS_Emitter.SubBullet.AccDirection;
		array2[25] = CS_Emitter.SubBullet.ScaleX;
		array2[26] = CS_Emitter.SubBullet.ScaleY;
		array2[27] = 0f;
		array2[28] = 0f;
		array2[29] = 0f;
		array2[30] = 0f;
		array2[31] = 0f;
		array2[32] = 0f;
		if (t < 1)
		{
			t = 1;
		}
		if ((CS_Emitter.Time - CS_Emitter.StartTime + 1) % t == 0)
		{
			loop++;
		}
		foreach (Event @event in EventList)
		{
			if (!CheckCondition(array, @event))
			{
				continue;
			}
			if (@event.special == 3)
			{
				if (@event.changevalue == 0)
				{
					@event.res = CS_Emitter.CS_Position.X - 4f;
				}
				if (@event.changevalue == 1)
				{
					@event.res = CS_Emitter.CS_Position.Y + 16f;
				}
				if (@event.changevalue == 6 || @event.changevalue == 24)
				{
					@event.res = Twopointangle(CS_Emitter.OriginalPosition.X - 4f, CS_Emitter.OriginalPosition.Y + 16f, CS_Emitter.OriginalPosition.X, CS_Emitter.OriginalPosition.Y) * 180f / (float)Math.PI;
				}
			}
			else if (@event.special == 4)
			{
				if (@event.changevalue == 0)
				{
					@event.res = CS_Emitter.MyPlane.OriginalPosition.X + 320f - 192f;
				}
				if (@event.changevalue == 1)
				{
					@event.res = CS_Emitter.MyPlane.OriginalPosition.Y + 240f - 224f;
				}
				if (@event.changevalue == 3 || @event.changevalue == 6 || @event.changevalue == 9 || @event.changevalue == 11 || @event.changevalue == 24)
				{
					@event.res = Twopointangle(CS_Emitter.MyPlane.OriginalPosition.X, CS_Emitter.MyPlane.OriginalPosition.Y, CS_Emitter.OriginalPosition.X, CS_Emitter.OriginalPosition.Y) * 180f / (float)Math.PI;
				}
			}
			if (@event.special == 1)
			{
				continue;
			}
			if (@event.special == 2)
			{
				CS_Emitter.ShootBullet();
				continue;
			}
			Execution execution = new Execution();
			if (@event.noloop)
			{
				continue;
			}
			if (@event.time > 0)
			{
				@event.time--;
				if (@event.time == 0)
				{
					@event.noloop = true;
				}
			}
			execution.parentid = CS_Emitter.LayerID;
			execution.id = CS_Emitter.ID;
			execution.change = @event.change;
			execution.changetype = @event.changetype;
			execution.changevalue = @event.changevalue;
			if (@event.rand != 0f)
			{
				execution.value = @event.res + @event.rand * (float)CS_Emitter.Ran.NextPMDouble();
			}
			else
			{
				execution.value = @event.res;
			}
			execution.region = array2[@event.changename].ToString();
			execution.time = @event.times;
			execution.ctime = execution.time;
			CS_Emitter.EventsExecutionList.Add(execution);
		}
	}

	public void Update(BaseObject_CS CS_Bullet)
	{
		float[] array = new float[3];
		float[] array2 = new float[21];
		array[0] = CS_Bullet.Time;
		array[1] = CS_Bullet.CS_Position.X;
		array[2] = CS_Bullet.CS_Position.Y;
		array2[0] = CS_Bullet.LifeTime;
		array2[1] = CS_Bullet.Type;
		array2[2] = CS_Bullet.ScaleWidth;
		array2[3] = CS_Bullet.ScaleLength;
		array2[4] = (int)CS_Bullet.ColorValue.R;
		array2[5] = (int)CS_Bullet.ColorValue.G;
		array2[6] = (int)CS_Bullet.ColorValue.B;
		array2[7] = CS_Bullet.TransparentValueF;
		array2[8] = (float)CS_Bullet.AngleDegree;
		array2[9] = CS_Bullet.Velocity;
		array2[10] = (float)CS_Bullet.DirectionDegree;
		array2[11] = CS_Bullet.AccelerateCS;
		array2[12] = (float)CS_Bullet.AccDirection;
		array2[13] = CS_Bullet.ScaleX;
		array2[14] = CS_Bullet.ScaleY;
		array2[15] = 0f;
		array2[16] = 0f;
		array2[17] = 0f;
		array2[18] = 0f;
		array2[19] = 0f;
		array2[20] = 0f;
		if (t < 1)
		{
			t = 1;
		}
		if ((CS_Bullet.Time + 1) % t == 0)
		{
			loop++;
		}
		foreach (Event @event in EventList)
		{
			if (@event.special2 == 1)
			{
				array[0] = CS_Bullet.StageData.TimeMain;
			}
			if (!CheckCondition(array, @event))
			{
				continue;
			}
			if (@event.special == 4 && (@event.changevalue == 10 || @event.changevalue == 12))
			{
				@event.res = Twopointangle(CS_Bullet.MyPlane.OriginalPosition.X, CS_Bullet.MyPlane.OriginalPosition.Y, CS_Bullet.OriginalPosition.X, CS_Bullet.OriginalPosition.Y) * 180f / (float)Math.PI;
			}
			Execution execution = new Execution();
			if (@event.noloop)
			{
				continue;
			}
			if (@event.time > 0)
			{
				@event.time--;
				if (@event.time == 0)
				{
					@event.noloop = true;
				}
			}
			execution.change = @event.change;
			execution.changetype = @event.changetype;
			execution.changevalue = @event.changevalue;
			if (@event.rand == 0f)
			{
				execution.value = @event.res;
			}
			else
			{
				execution.value = @event.res + @event.rand * (float)CS_Bullet.StageData.Ran.NextPMDouble();
			}
			execution.region = array2[@event.changename].ToString();
			execution.time = @event.times;
			execution.ctime = execution.time;
			CS_Bullet.EventsExecutionList.Add(execution);
		}
	}

	private bool CheckCondition(float[] conditions, Event ev)
	{
		bool flag = false;
		bool flag2 = false;
		if (ev.opreator == ">")
		{
			flag = conditions[ev.contype] > float.Parse(ev.condition) + (float)(loop * addtime);
		}
		else if (ev.opreator == "<")
		{
			flag = conditions[ev.contype] < float.Parse(ev.condition) + (float)(loop * addtime);
		}
		else if (ev.opreator == "=")
		{
			flag = conditions[ev.contype] == float.Parse(ev.condition) + (float)(loop * addtime);
		}
		if (ev.opreator2 == ">")
		{
			flag2 = conditions[ev.contype2] > float.Parse(ev.condition2) + (float)(loop * addtime);
		}
		else if (ev.opreator2 == "<")
		{
			flag2 = conditions[ev.contype2] < float.Parse(ev.condition2) + (float)(loop * addtime);
		}
		else if (ev.opreator2 == "=")
		{
			flag2 = conditions[ev.contype2] == float.Parse(ev.condition2) + (float)(loop * addtime);
		}
		if (ev.collector == "且")
		{
			return flag & flag2;
		}
		if (ev.collector == "或")
		{
			return flag | flag2;
		}
		return flag;
	}

	private float Twopointangle(float x2, float y2, float x1, float y1)
	{
		float num = ((x2 - x1 == 0f) ? ((float)Math.Atan((y2 - y1) / (x2 - x1 + 0.1f))) : ((float)Math.Atan((y2 - y1) / (x2 - x1))));
		if (x2 - x1 < 0f)
		{
			num += (float)Math.PI;
		}
		return num;
	}

	public object Clone()
	{
		EventGroup ev = (EventGroup)MemberwiseClone();
		ev.EventList = new List<Event>();
		EventList.ForEach((Event x) =>
		{
			ev.EventList.Add((Event)x.Clone());
		});
		return ev;
	}
}
