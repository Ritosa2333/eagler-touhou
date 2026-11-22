using System;
using System.Drawing;

namespace Shooting;

[Serializable]
public class Execution
{
	public int parentid;

	public int id;

	public int change;

	public int changetype;

	public int changevalue;

	public string region;

	public float value;

	public int time;

	public int ctime;

	public void Update(BaseEmitter_CS objects)
	{
		switch (changevalue)
		{
		case 0:
		{
			float x = objects.CS_Position.X;
			objects.CS_Position = new PointF(ChangeValue(x), objects.CS_Position.Y);
			break;
		}
		case 1:
		{
			float y = objects.CS_Position.Y;
			objects.CS_Position = new PointF(objects.CS_Position.X, ChangeValue(y));
			break;
		}
		case 2:
			objects.EmitRadius = ChangeValue(objects.EmitRadius);
			break;
		case 3:
			objects.RadiusDirection = ChangeValue(objects.RadiusDirection);
			break;
		case 4:
			objects.Way = ChangeValue(objects.Way);
			break;
		case 5:
			objects.Circle = ChangeValue(objects.Circle);
			break;
		case 6:
			objects.EmitDirection = ChangeValue(objects.EmitDirection);
			break;
		case 7:
			objects.Range = ChangeValue(objects.Range);
			break;
		case 8:
			objects.Velocity = ChangeValue(objects.Velocity);
			break;
		case 9:
			objects.DirectionDegree = ChangeValue(objects.DirectionDegree);
			break;
		case 10:
			objects.AccelerateCS = ChangeValue(objects.AccelerateCS);
			break;
		case 11:
			objects.AccDirection = ChangeValue(objects.AccDirection);
			break;
		case 12:
			objects.SubBullet.LifeTime = ChangeValue(objects.SubBullet.LifeTime);
			break;
		case 13:
			objects.SubBullet.Type = ChangeValue(objects.SubBullet.Type);
			break;
		case 14:
			objects.SubBullet.ScaleWidth = ChangeValue(objects.SubBullet.ScaleWidth);
			break;
		case 15:
			objects.SubBullet.ScaleLength = ChangeValue(objects.SubBullet.ScaleLength);
			break;
		case 16:
		{
			int r = objects.SubBullet.ColorValue.R;
			r = ChangeValue(r);
			r = ((r >= 0) ? r : 0);
			r = ((r > 255) ? 255 : r);
			objects.SubBullet.ColorValue = Color.FromArgb(r, objects.SubBullet.ColorValue.G, objects.SubBullet.ColorValue.B);
			break;
		}
		case 17:
		{
			int g = objects.SubBullet.ColorValue.G;
			g = ChangeValue(g);
			g = ((g >= 0) ? g : 0);
			g = ((g > 255) ? 255 : g);
			objects.SubBullet.ColorValue = Color.FromArgb(objects.SubBullet.ColorValue.R, g, objects.SubBullet.ColorValue.B);
			break;
		}
		case 18:
		{
			int b = objects.SubBullet.ColorValue.B;
			b = ChangeValue(b);
			b = ((b >= 0) ? b : 0);
			b = ((b > 255) ? 255 : b);
			objects.SubBullet.ColorValue = Color.FromArgb(objects.SubBullet.ColorValue.R, objects.SubBullet.ColorValue.G, b);
			break;
		}
		case 19:
			objects.SubBullet.TransparentValueF = ChangeValue(objects.SubBullet.TransparentValueF);
			break;
		case 20:
			objects.SubBullet.AngleDegree = ChangeValue(objects.SubBullet.AngleDegree);
			break;
		case 21:
			objects.SubBullet.Velocity = ChangeValue(objects.SubBullet.Velocity);
			break;
		case 22:
			objects.SubBullet.DirectionDegree = ChangeValue(objects.SubBullet.DirectionDegree);
			break;
		case 23:
			objects.SubBullet.AccelerateCS = ChangeValue(objects.SubBullet.AccelerateCS);
			break;
		case 24:
			objects.SubBullet.AccDirection = ChangeValue(objects.SubBullet.AccDirection);
			break;
		case 25:
			objects.SubBullet.ScaleX = ChangeValue(objects.SubBullet.ScaleX);
			break;
		case 26:
			objects.SubBullet.ScaleY = ChangeValue(objects.SubBullet.ScaleY);
			break;
		case 27:
			objects.SubBullet.BeginningEffect = value > 0f;
			break;
		case 28:
			objects.SubBullet.EndingEffect = value > 0f;
			break;
		case 29:
			objects.SubBullet.Active = value > 0f;
			break;
		case 30:
			objects.SubBullet.Ghosting = value > 0f;
			break;
		case 31:
			objects.SubBullet.OutBound = value > 0f;
			break;
		case 32:
			objects.SubBullet.UnRemoveable = value > 0f;
			break;
		}
		ctime--;
		if (((changetype == 2) & (ctime == -1)) || ((changetype != 2) & (ctime == 0)))
		{
			objects.EventsExecutionList.Remove(this);
		}
	}

	public void Update(BaseObject_CS objects)
	{
		switch (changevalue)
		{
		case 0:
			objects.LifeTime = ChangeValue(objects.LifeTime);
			break;
		case 1:
			objects.Type = ChangeValue(objects.Type);
			break;
		case 2:
			objects.ScaleWidth = ChangeValue(objects.ScaleWidth);
			break;
		case 3:
			objects.ScaleLength = ChangeValue(objects.ScaleLength);
			break;
		case 4:
		{
			int r = objects.ColorValue.R;
			r = ChangeValue(r);
			r = ((r >= 0) ? r : 0);
			r = ((r > 255) ? 255 : r);
			objects.ColorValue = Color.FromArgb(r, objects.ColorValue.G, objects.ColorValue.B);
			break;
		}
		case 5:
		{
			int g = objects.ColorValue.G;
			g = ChangeValue(g);
			g = ((g >= 0) ? g : 0);
			g = ((g > 255) ? 255 : g);
			objects.ColorValue = Color.FromArgb(objects.ColorValue.R, g, objects.ColorValue.B);
			break;
		}
		case 6:
		{
			int b = objects.ColorValue.B;
			b = ChangeValue(b);
			b = ((b >= 0) ? b : 0);
			b = ((b > 255) ? 255 : b);
			objects.ColorValue = Color.FromArgb(objects.ColorValue.R, objects.ColorValue.G, b);
			break;
		}
		case 7:
			objects.TransparentValueF = ChangeValue(objects.TransparentValueF);
			break;
		case 8:
			objects.AngleDegree = ChangeValue(objects.AngleDegree);
			break;
		case 9:
			objects.Velocity = ChangeValue(objects.Velocity);
			break;
		case 10:
			objects.DirectionDegree = ChangeValue(objects.DirectionDegree);
			break;
		case 11:
			objects.AccelerateCS = ChangeValue(objects.AccelerateCS);
			break;
		case 12:
			objects.AccDirection = ChangeValue(objects.AccDirection);
			break;
		case 13:
			objects.ScaleX = ChangeValue(objects.ScaleX);
			break;
		case 14:
			objects.ScaleY = ChangeValue(objects.ScaleY);
			break;
		case 15:
			objects.BeginningEffect = value > 0f;
			break;
		case 16:
			objects.EndingEffect = value > 0f;
			break;
		case 17:
			objects.Active = value > 0f;
			break;
		case 18:
			objects.Ghosting = value > 0f;
			break;
		case 19:
			objects.OutBound = value > 0f;
			break;
		case 20:
			objects.UnRemoveable = value > 0f;
			break;
		}
		ctime--;
		if (((changetype == 2) & (ctime == -1)) || ((changetype != 2) & (ctime == 0)))
		{
			objects.EventsExecutionList.Remove(this);
		}
	}

	private int ChangeValue(int Value)
	{
		return (int)ChangeValue((double)Value);
	}

	private float ChangeValue(float Value)
	{
		return (float)ChangeValue((double)Value);
	}

	private double ChangeValue(double Value)
	{
		if (changetype == 0)
		{
			if (change == 0)
			{
				return (Value * (double)((float)ctime - 1f) + (double)value) / (double)(float)ctime;
			}
			if (change == 1)
			{
				return Value + (double)(value / (float)time);
			}
			return Value - (double)(value / (float)time);
		}
		if (changetype == 1)
		{
			if (change == 0)
			{
				return value;
			}
			if (change == 1)
			{
				return Value + (double)value;
			}
			return Value - (double)value;
		}
		if (change == 0)
		{
			return float.Parse(region) + (value - float.Parse(region)) * (float)Math.Sin(Math.PI * 2.0 / (double)(float)time * (double)((float)time - (float)ctime));
		}
		if (change == 1)
		{
			return float.Parse(region) + value * (float)Math.Sin(Math.PI * 2.0 / (double)(float)time * (double)((float)time - (float)ctime));
		}
		return float.Parse(region) - value * (float)Math.Sin(Math.PI * 2.0 / (double)(float)time * (double)((float)time - (float)ctime));
	}
}
