using System;
using System.Drawing;

namespace Shooting;

public class BulletPicEmitter01 : BaseBulleEmitter
{
	private bool[,] BulletPic;

	private BaseObject Bullet;

	private int indexY;

	private int length;

	private byte ColorType;

	public BulletPicEmitter01(StageDataPackage StageData, string BPicName, BaseObject Bullet, PointF OriginalPosition, double Direction, byte ColorType)
		: base(StageData)
	{
		base.OriginalPosition = OriginalPosition;
		BulletPic = StageData.GlobalData.BulletPicDictionary[BPicName];
		this.Bullet = Bullet;
		this.ColorType = ColorType;
		length = (int)Math.Sqrt(BulletPic.Length);
		LifeTime = length * 2 + 10;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time >= LifeTime)
		{
			BulletList.ForEach((BaseBullet_Touhou x) =>
			{
				x.Enabled = true;
			});
		}
		if (Time % 2 != 0 || indexY >= length)
		{
			return;
		}
		for (int num = 0; num < length; num++)
		{
			if (BulletPic[num, indexY])
			{
				PointF originalPosition = new PointF(OriginalPosition.X + (float)(num - length / 2) * 5f, OriginalPosition.Y + (float)(indexY - length / 2) * 5f);
				BaseBullet_Touhou baseBullet_Touhou = new BaseBullet_Touhou(StageData, null, originalPosition, 0f, 0.0);
				baseBullet_Touhou.Copy(Bullet);
				baseBullet_Touhou.OriginalPosition = originalPosition;
				baseBullet_Touhou.Velocity = (float)baseBullet_Touhou.GetDistance(OriginalPosition) * 3f / (float)length;
				baseBullet_Touhou.DirectionDegree = Ran.Next(360);
				baseBullet_Touhou.Enabled = false;
				StageData.SoundPlay("se_tan00a.wav");
			}
		}
		indexY++;
	}

	public override void Shoot()
	{
	}
}
