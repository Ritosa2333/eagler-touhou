using System;
using System.Drawing;

namespace Shooting;

public class Shinmyoumaru : BaseSubPlane
{
	public float IndexX { get; set; }

	public int IndexY { get; set; }

	public Shinmyoumaru(StageDataPackage StageData, PointF Position)
		: base(StageData, "ReimuSubPlane", Position)
	{
		AngularVelocityDegree = 5f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		TextureCtrl();
	}

	public void TextureCtrl()
	{
		if (!KClass.Key_Shift)
		{
			TxtureObject = TextureObjectDictionary["ReimuSubPlane"];
			return;
		}
		Angle = Math.PI / 2.0;
		int indexY = IndexY;
		if (Vx > 0.5f)
		{
			IndexY = 2;
			if (IndexX < 4f)
			{
				IndexX += 0.5f;
			}
			else
			{
				IndexX = 4 + Time % 24 / 6;
			}
		}
		else if (Vx < -0.5f)
		{
			IndexY = 1;
			if (IndexX < 4f)
			{
				IndexX += 0.5f;
			}
			else
			{
				IndexX = 4 + Time % 24 / 6;
			}
		}
		else
		{
			IndexY = 0;
			IndexX = Time % 48 / 6;
		}
		if (indexY != IndexY)
		{
			IndexX = 1f;
		}
		if (IndexX > 7f)
		{
			IndexX = 7f;
		}
		string key = "Maru" + IndexY + (int)IndexX;
		TxtureObject = TextureObjectDictionary[key];
	}

	public override void Shoot()
	{
		if (KClass.Key_Shift)
		{
			new BaseMyBullet_Touhou(StageData, "MaruBullet", OriginalPosition, 18f, ShootDirection + 0.0523598802906103)
			{
				Angle = 1.5707963705062866,
				Damage = 5
			};
			new BaseMyBullet_Touhou(StageData, "MaruBullet", OriginalPosition, 18f, ShootDirection - 0.0523598802906103)
			{
				Angle = 1.5707963705062866,
				Damage = 5
			};
		}
		else
		{
			new BulletGuidance(StageData, "ReimuSubBullet", Position, 8f, ShootDirection)
			{
				TransparentValueF = 180f,
				Angle = 1.5707963705062866,
				Damage = 9
			};
		}
	}

	public override void Show()
	{
		if (!Enabled || TxtureObject == null)
		{
			return;
		}
		if (!KClass.Key_Shift)
		{
			double num = (double)(-Time * 5 % 360) * Math.PI / 180.0;
			if (MyPlane.Power >= 400)
			{
				SpriteMain.Draw2D(TxtureObject, 1.3f, 1.3f, 0f - (float)num, Position, Color.FromArgb(TransparentValue * 6 / 10, ColorValue), Mirrored);
			}
			SpriteMain.Draw2D(TxtureObject, ScaleWidth, ScaleLength, (float)num, Position, Color.FromArgb(TransparentValue, ColorValue), Mirrored);
		}
		else
		{
			base.Show();
		}
	}
}
