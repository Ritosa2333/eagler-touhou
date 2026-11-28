using System;
using System.Drawing;
using SlimDX;

namespace Shooting;

public class AutoPlane(StageDataPackage StageData, Point Position) : BaseMyPlane(StageData, Position)
{
	private int CtrlCircle = 1;

	private KeyClass KC = new KeyClass();

	public override void Ctrl()
	{
		//IL_003e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0043: Unknown result type (might be due to invalid IL or missing references)
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_004d: Unknown result type (might be due to invalid IL or missing references)
		if (Time % CtrlCircle == 0)
		{
			KC = new KeyClass();
			KC.Key_Z = true;
			double num = 0.0;
			double num2 = 0.0;
			Vector2 val = GenerateMove();
			num = 0f - val.X;
			num2 = 0f - val.Y;
			if (val.Length() < 80f)
			{
				KC.Key_Shift = true;
			}
			if (num > 1.0)
			{
				KC.ArrowLeft = true;
			}
			else if (num < -1.0)
			{
				KC.ArrowRight = true;
			}
			if (num2 > 1.0)
			{
				KC.ArrowUp = true;
			}
			else if (num2 < -1.0)
			{
				KC.ArrowDown = true;
			}
		}
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			if (x.HitCheck(this, x.Region) || DeadTime > Time)
			{
				KC.Key_X = true;
			}
		});
		StageData.GlobalData.KClass.Hex2Key(KC.Key2Hex());
		base.Ctrl();
	}

	private double GenerateDanger(PointF OriginalPos)
	{
		double tmp = 0.0;
		double d;
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			//IL_0018: Unknown result type (might be due to invalid IL or missing references)
			//IL_003b: Unknown result type (might be due to invalid IL or missing references)
			//IL_0040: Unknown result type (might be due to invalid IL or missing references)
			//IL_0041: Unknown result type (might be due to invalid IL or missing references)
			//IL_0042: Unknown result type (might be due to invalid IL or missing references)
			//IL_0047: Unknown result type (might be due to invalid IL or missing references)
			//IL_00c1: Unknown result type (might be due to invalid IL or missing references)
			//IL_00c6: Unknown result type (might be due to invalid IL or missing references)
			//IL_00cb: Unknown result type (might be due to invalid IL or missing references)
			//IL_00cd: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e6: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e8: Unknown result type (might be due to invalid IL or missing references)
			//IL_00ed: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f2: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f3: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f4: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f5: Unknown result type (might be due to invalid IL or missing references)
			//IL_00fa: Unknown result type (might be due to invalid IL or missing references)
			Vector2 val = new Vector2(OriginalPos.X, OriginalPos.Y);
			Vector2 val2 = new Vector2(x.OriginalPosition.X, x.OriginalPosition.Y);
			Vector2 val3 = val - val2;
			d = val3.Length() - (float)Region - (float)x.Region;
			if (d < 200.0)
			{
				tmp += 7000.0 / d / d;
				Vector2 val4 = x.Velocity * new Vector2((float)Math.Cos(x.Direction), (float)Math.Sin(x.Direction));
				val2 += (float)GetDistance(OriginalPos) / 5f * val4;
				val3 = val - val2;
				d = val3.Length() - (float)Region - (float)x.Region;
				tmp += 3000.0 / d / d;
			}
		});
		EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
		{
			//IL_0094: Unknown result type (might be due to invalid IL or missing references)
			//IL_00b7: Unknown result type (might be due to invalid IL or missing references)
			//IL_00bc: Unknown result type (might be due to invalid IL or missing references)
			//IL_00bd: Unknown result type (might be due to invalid IL or missing references)
			//IL_00c2: Unknown result type (might be due to invalid IL or missing references)
			if (x.OriginalPosition.X > 0f && x.OriginalPosition.X < (float)BoundRect.Width && x.OriginalPosition.Y > 0f && x.OriginalPosition.Y < (float)BoundRect.Height)
			{
				Vector2 val = new Vector2(OriginalPos.X, OriginalPos.Y);
				Vector2 val2 = new Vector2(x.OriginalPosition.X, x.OriginalPosition.Y);
				Vector2 val3 = val - val2;
				d = val3.Length() - (float)Region - (float)x.Region;
				tmp += 10000.0 / d / d;
			}
		});
		if (Boss != null)
		{
			d = Boss.GetDistance(OriginalPos) - (double)Boss.Region - (double)Region;
			tmp += 10000.0 / d / d;
		}
		return tmp;
	}

	private Vector2 GenerateDestPoint(int div, int count, float x, float y)
	{
		//IL_0000: Unknown result type (might be due to invalid IL or missing references)
		//IL_0005: Unknown result type (might be due to invalid IL or missing references)
		//IL_013f: Unknown result type (might be due to invalid IL or missing references)
		//IL_011f: Unknown result type (might be due to invalid IL or missing references)
		//IL_00dd: Unknown result type (might be due to invalid IL or missing references)
		Vector2 result = Vector2.Zero;
		double num = 1000000000.0;
		double num2 = 0.0;
		x -= (float)(div * (count - 1) / 2);
		y -= (float)(div * (count - 1) / 2);
		if (x < 0f)
		{
			x = 0f;
		}
		else if (x > (float)(BoundRect.Width - div * (count - 1)))
		{
			x = BoundRect.Width - div * (count - 1);
		}
		if (y < 0f)
		{
			y = 0f;
		}
		else if (y > (float)(BoundRect.Height - div * (count - 1)))
		{
			y = BoundRect.Height - div * (count - 1);
		}
		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < count; j++)
			{
				double num3 = GenerateDanger(new PointF(x, y));
				if (num3 < num)
				{
					num = num3;
					result = new Vector2(x, y);
				}
				if (num3 > num2)
				{
					num2 = num3;
				}
				x += (float)div;
			}
			y += (float)div;
			x -= (float)(div * count);
		}
		if (num2 > 2.0)
		{
			return result;
		}
		return new Vector2(OriginalPosition.X, OriginalPosition.Y);
	}

	private Vector2 GenerateMove()
	{
		//IL_000e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0013: Unknown result type (might be due to invalid IL or missing references)
		//IL_0126: Unknown result type (might be due to invalid IL or missing references)
		//IL_012b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0130: Unknown result type (might be due to invalid IL or missing references)
		//IL_0136: Unknown result type (might be due to invalid IL or missing references)
		//IL_013c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0141: Unknown result type (might be due to invalid IL or missing references)
		//IL_0145: Unknown result type (might be due to invalid IL or missing references)
		//IL_014b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0151: Unknown result type (might be due to invalid IL or missing references)
		//IL_0156: Unknown result type (might be due to invalid IL or missing references)
		//IL_0177: Unknown result type (might be due to invalid IL or missing references)
		//IL_006f: Unknown result type (might be due to invalid IL or missing references)
		//IL_009e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00aa: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ea: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ef: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f8: Unknown result type (might be due to invalid IL or missing references)
		//IL_00fa: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ff: Unknown result type (might be due to invalid IL or missing references)
		//IL_01b6: Unknown result type (might be due to invalid IL or missing references)
		//IL_01bb: Unknown result type (might be due to invalid IL or missing references)
		//IL_01bc: Unknown result type (might be due to invalid IL or missing references)
		//IL_01bd: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c6: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d0: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d7: Unknown result type (might be due to invalid IL or missing references)
		//IL_01dc: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e8: Unknown result type (might be due to invalid IL or missing references)
		//IL_0189: Unknown result type (might be due to invalid IL or missing references)
		//IL_018a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0190: Unknown result type (might be due to invalid IL or missing references)
		//IL_0195: Unknown result type (might be due to invalid IL or missing references)
		//IL_0249: Unknown result type (might be due to invalid IL or missing references)
		//IL_029c: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a1: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a6: Unknown result type (might be due to invalid IL or missing references)
		//IL_02ad: Unknown result type (might be due to invalid IL or missing references)
		//IL_02b3: Unknown result type (might be due to invalid IL or missing references)
		//IL_02b8: Unknown result type (might be due to invalid IL or missing references)
		//IL_02bd: Unknown result type (might be due to invalid IL or missing references)
		//IL_0202: Unknown result type (might be due to invalid IL or missing references)
		//IL_0236: Unknown result type (might be due to invalid IL or missing references)
		//IL_023b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0240: Unknown result type (might be due to invalid IL or missing references)
		//IL_02c4: Unknown result type (might be due to invalid IL or missing references)
		//IL_02ca: Unknown result type (might be due to invalid IL or missing references)
		//IL_02cf: Unknown result type (might be due to invalid IL or missing references)
		//IL_02d4: Unknown result type (might be due to invalid IL or missing references)
		//IL_02da: Unknown result type (might be due to invalid IL or missing references)
		Vector2 forceSum = Vector2.Zero;
		float d;
		BulletList.ForEach((BaseBullet_Touhou x) =>
		{
			//IL_0028: Unknown result type (might be due to invalid IL or missing references)
			//IL_004b: Unknown result type (might be due to invalid IL or missing references)
			//IL_0050: Unknown result type (might be due to invalid IL or missing references)
			//IL_0051: Unknown result type (might be due to invalid IL or missing references)
			//IL_0052: Unknown result type (might be due to invalid IL or missing references)
			//IL_0057: Unknown result type (might be due to invalid IL or missing references)
			//IL_0091: Unknown result type (might be due to invalid IL or missing references)
			//IL_00a5: Unknown result type (might be due to invalid IL or missing references)
			//IL_00aa: Unknown result type (might be due to invalid IL or missing references)
			//IL_00ad: Unknown result type (might be due to invalid IL or missing references)
			//IL_00b2: Unknown result type (might be due to invalid IL or missing references)
			//IL_00b3: Unknown result type (might be due to invalid IL or missing references)
			//IL_00b8: Unknown result type (might be due to invalid IL or missing references)
			//IL_00db: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e0: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e5: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e7: Unknown result type (might be due to invalid IL or missing references)
			//IL_00ed: Unknown result type (might be due to invalid IL or missing references)
			//IL_00ef: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f4: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f9: Unknown result type (might be due to invalid IL or missing references)
			//IL_00fa: Unknown result type (might be due to invalid IL or missing references)
			//IL_00fb: Unknown result type (might be due to invalid IL or missing references)
			//IL_00fc: Unknown result type (might be due to invalid IL or missing references)
			//IL_0101: Unknown result type (might be due to invalid IL or missing references)
			//IL_012b: Unknown result type (might be due to invalid IL or missing references)
			//IL_013f: Unknown result type (might be due to invalid IL or missing references)
			//IL_0144: Unknown result type (might be due to invalid IL or missing references)
			//IL_0147: Unknown result type (might be due to invalid IL or missing references)
			//IL_014c: Unknown result type (might be due to invalid IL or missing references)
			//IL_014d: Unknown result type (might be due to invalid IL or missing references)
			//IL_0152: Unknown result type (might be due to invalid IL or missing references)
			//IL_0157: Unknown result type (might be due to invalid IL or missing references)
			//IL_015d: Unknown result type (might be due to invalid IL or missing references)
			//IL_015f: Unknown result type (might be due to invalid IL or missing references)
			//IL_0164: Unknown result type (might be due to invalid IL or missing references)
			//IL_0169: Unknown result type (might be due to invalid IL or missing references)
			//IL_016a: Unknown result type (might be due to invalid IL or missing references)
			//IL_016b: Unknown result type (might be due to invalid IL or missing references)
			//IL_016c: Unknown result type (might be due to invalid IL or missing references)
			//IL_0171: Unknown result type (might be due to invalid IL or missing references)
			//IL_019b: Unknown result type (might be due to invalid IL or missing references)
			//IL_01af: Unknown result type (might be due to invalid IL or missing references)
			//IL_01b4: Unknown result type (might be due to invalid IL or missing references)
			//IL_01b7: Unknown result type (might be due to invalid IL or missing references)
			//IL_01bc: Unknown result type (might be due to invalid IL or missing references)
			//IL_01bd: Unknown result type (might be due to invalid IL or missing references)
			//IL_01c2: Unknown result type (might be due to invalid IL or missing references)
			Vector2 val8 = new Vector2(OriginalPosition.X, OriginalPosition.Y);
			Vector2 val9 = new Vector2(x.OriginalPosition.X, x.OriginalPosition.Y);
			Vector2 val10 = val8 - val9;
			d = val10.Length() - (float)Region - (float)x.Region;
			if (d < 200f)
			{
				val10.Normalize();
				val10 *= 5000f / d / d;
				forceSum += val10;
				Vector2 val11 = x.Velocity * new Vector2((float)Math.Cos(x.Direction), (float)Math.Sin(x.Direction));
				val9 += 3f * val11;
				val10 = val8 - val9;
				d = val10.Length() - (float)Region - (float)x.Region;
				val10.Normalize();
				val10 *= 5000f / d / d;
				forceSum += val10;
				val9 += 6f * val11;
				val10 = val8 - val9;
				d = val10.Length() - (float)Region - (float)x.Region;
				val10.Normalize();
				val10 *= 2000f / d / d;
				forceSum += val10;
			}
		});
		EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
		{
			//IL_00a4: Unknown result type (might be due to invalid IL or missing references)
			//IL_00c7: Unknown result type (might be due to invalid IL or missing references)
			//IL_00cc: Unknown result type (might be due to invalid IL or missing references)
			//IL_00cd: Unknown result type (might be due to invalid IL or missing references)
			//IL_00ce: Unknown result type (might be due to invalid IL or missing references)
			//IL_00d3: Unknown result type (might be due to invalid IL or missing references)
			//IL_00fd: Unknown result type (might be due to invalid IL or missing references)
			//IL_0111: Unknown result type (might be due to invalid IL or missing references)
			//IL_0116: Unknown result type (might be due to invalid IL or missing references)
			//IL_0119: Unknown result type (might be due to invalid IL or missing references)
			//IL_011e: Unknown result type (might be due to invalid IL or missing references)
			//IL_011f: Unknown result type (might be due to invalid IL or missing references)
			//IL_0124: Unknown result type (might be due to invalid IL or missing references)
			//IL_0147: Unknown result type (might be due to invalid IL or missing references)
			//IL_014c: Unknown result type (might be due to invalid IL or missing references)
			//IL_0151: Unknown result type (might be due to invalid IL or missing references)
			//IL_0153: Unknown result type (might be due to invalid IL or missing references)
			//IL_0159: Unknown result type (might be due to invalid IL or missing references)
			//IL_015b: Unknown result type (might be due to invalid IL or missing references)
			//IL_0160: Unknown result type (might be due to invalid IL or missing references)
			//IL_0165: Unknown result type (might be due to invalid IL or missing references)
			//IL_0166: Unknown result type (might be due to invalid IL or missing references)
			//IL_0167: Unknown result type (might be due to invalid IL or missing references)
			//IL_016c: Unknown result type (might be due to invalid IL or missing references)
			//IL_0196: Unknown result type (might be due to invalid IL or missing references)
			//IL_01aa: Unknown result type (might be due to invalid IL or missing references)
			//IL_01af: Unknown result type (might be due to invalid IL or missing references)
			//IL_01b2: Unknown result type (might be due to invalid IL or missing references)
			//IL_01b7: Unknown result type (might be due to invalid IL or missing references)
			//IL_01b8: Unknown result type (might be due to invalid IL or missing references)
			//IL_01bd: Unknown result type (might be due to invalid IL or missing references)
			if (x.OriginalPosition.X > 0f && x.OriginalPosition.X < (float)BoundRect.Width && x.OriginalPosition.Y > 0f && x.OriginalPosition.Y < (float)BoundRect.Height)
			{
				Vector2 val8 = new Vector2(OriginalPosition.X, OriginalPosition.Y);
				Vector2 val9 = new Vector2(x.OriginalPosition.X, x.OriginalPosition.Y);
				Vector2 val10 = val8 - val9;
				d = val10.Length() - (float)Region - (float)x.Region;
				val10.Normalize();
				val10 *= 5000f / d / d;
				forceSum += val10;
				Vector2 val11 = x.Velocity * new Vector2((float)Math.Cos(x.Direction), (float)Math.Sin(x.Direction));
				val9 += 5f * val11;
				val10 = val8 - val9;
				d = val10.Length() - (float)Region - (float)x.Region;
				val10.Normalize();
				val10 *= 5000f / d / d;
				forceSum += val10;
			}
		});
		if (Boss != null)
		{
			Vector2 val = new Vector2(OriginalPosition.X, OriginalPosition.Y);
			Vector2 val2 = new Vector2(Boss.OriginalPosition.X, Boss.OriginalPosition.Y);
			Vector2 val3 = val - val2;
			d = val3.Length() - (float)Region - (float)Boss.Region;
			val3.Normalize();
			val3 *= 10000f / d / d;
			forceSum += val3;
		}
		Vector2 val4 = GenerateDestPoint(40, 4, OriginalPosition.X, OriginalPosition.Y);
		val4 = GenerateDestPoint(10, 4, val4.X, val4.Y);
		val4 = GenerateDestPoint(2, 5, val4.X, val4.Y);
		Vector2 val5 = new Vector2(DestPoint.X, DestPoint.Y);
		if (Time > 300)
		{
			val4 = Vector2.Lerp(val4, val5, 0.5f);
		}
		Vector2 val6 = new Vector2(OriginalPosition.X, OriginalPosition.Y);
		Vector2 val7 = val4 - val6;
		forceSum += 1f * val7;
		DestPoint = new PointF(val4.X, val4.Y);
		if (Boss != null)
		{
			forceSum += new Vector2(5f * (Boss.OriginalPosition.X - OriginalPosition.X), 0f);
		}
		else
		{
			forceSum += new Vector2(0.1f * ((float)(BoundRect.Width / 2) - OriginalPosition.X), 0.1f * ((float)BoundRect.Height - OriginalPosition.Y));
			forceSum += GenerateEnemyBeat();
		}
		forceSum += GenerateItemGet();
		return forceSum;
	}

	private Vector2 GenerateEnemyBeat()
	{
		//IL_000d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0012: Unknown result type (might be due to invalid IL or missing references)
		//IL_0069: Unknown result type (might be due to invalid IL or missing references)
		//IL_0064: Unknown result type (might be due to invalid IL or missing references)
		Vector2 result = Vector2.Zero;
		float tmp = 1000f;
		EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
		{
			if (x.OriginalPosition.Y > 0f && x.OriginalPosition.Y < 300f && x.HealthPoint < 20000f && Math.Abs(x.OriginalPosition.X - OriginalPosition.X) < tmp)
			{
				tmp = x.OriginalPosition.X;
			}
		});
		if (tmp < 1000f)
		{
			result = new Vector2(10f * (tmp - OriginalPosition.X), 0f);
		}
		return result;
	}

	private Vector2 GenerateItemGet()
	{
		//IL_0015: Unknown result type (might be due to invalid IL or missing references)
		//IL_0023: Unknown result type (might be due to invalid IL or missing references)
		//IL_0075: Unknown result type (might be due to invalid IL or missing references)
		//IL_006f: Unknown result type (might be due to invalid IL or missing references)
		if (BulletList.Count > 10)
		{
			return Vector2.Zero;
		}
		if (Boss != null)
		{
			return Vector2.Zero;
		}
		int itemCount = 0;
		ItemList.ForEach((BaseItem x) =>
		{
			if (!x.Obtain)
			{
				itemCount++;
			}
		});
		if (itemCount > 5)
		{
			return new Vector2(0f, 2f * (110f - OriginalPosition.Y));
		}
		return Vector2.Zero;
	}

	public override void Show()
	{
		SpriteMain.Draw2D(TextureObjectDictionary["Region"], 0.1f, 0f, new PointF(DestPoint.X + (float)BoundRect.Left, DestPoint.Y + (float)BoundRect.Top), Color.White);
		base.Show();
	}
}
