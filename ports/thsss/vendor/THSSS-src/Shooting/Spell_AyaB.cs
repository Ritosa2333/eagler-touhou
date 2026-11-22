using System;
using System.Drawing;

namespace Shooting;

public class Spell_AyaB : BaseSpell
{
	private PointF tmpPoint;

	private int tmpRegion;

	private int Circle = 32;

	private bool Left = true;

	public Spell_AyaB(StageDataPackage StageData)
	{
		base.StageData = StageData;
		OriginalPosition = MyPlane.OriginalPosition;
		tmpPoint = MyPlane.OriginalPosition;
		LifeTime = 270;
		SpellList.Add(this);
		new BaseEffect(StageData, null, Position, 0f, Math.PI / 2.0)
		{
			TxtureObject = MyPlane.TxtureObject,
			TransparentVelocity = -20f,
			ScaleVelocity = 0.1f
		};
		new BackgroundBlack(StageData, Position);
		new BackgroundMySC(StageData, "76-4")
		{
			LifeTime = 350,
			MaxTransparent = 180,
			TransparentValueF = 0f,
			TransparentVelocity = 4f,
			ColorValue = Color.Green
		};
		new BackgroundMySC(StageData, "76-4")
		{
			LifeTime = 350,
			MaxTransparent = 150,
			TransparentValueF = 0f,
			TransparentVelocity = 4f,
			ColorValue = Color.Green,
			AngularVelocityDegree = -0.5f,
			Active = true
		};
		tmpRegion = MyPlane.Region;
		MyPlane.Region = -100;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > 64 && Time <= 96)
		{
			Circle = 16;
		}
		else if (Time > 96 && Time <= 112)
		{
			Circle = 8;
		}
		if (Time % Circle == 0)
		{
			Spell_AyaB_Laser spell_AyaB_Laser = new Spell_AyaB_Laser(StageData, OriginalPosition, Left);
			if (Time <= 96)
			{
				new BaseEffect(StageData, "Ayaya", new PointF(spell_AyaB_Laser.Position.X + 360f * (float)Math.Cos(spell_AyaB_Laser.Direction), spell_AyaB_Laser.Position.Y + 360f * (float)Math.Sin(spell_AyaB_Laser.Direction)), spell_AyaB_Laser.Velocity, spell_AyaB_Laser.Direction)
				{
					Active = false,
					Mirrored = !Left,
					Scale = 1.5f,
					GhostingCount = 5
				};
			}
			Left = !Left;
			StageData.SoundPlay("se_tan01.wav");
			StageData.VibrateStart(8);
		}
		ItemList.ForEach((BaseItem x) =>
		{
			x.Region = 1000;
			x.Doubled = true;
		});
		if (Time == LifeTime)
		{
			ItemList.ForEach((BaseItem x) =>
			{
				x.Obtain = true;
			});
			new BaseEffect(StageData, null, MyPlane.Position, 0f, Math.PI / 2.0)
			{
				TxtureObject = MyPlane.TxtureObject,
				Scale = 3f,
				ScaleVelocity = -0.1f,
				LifeTime = 20,
				TransparentValueF = 0f,
				TransparentVelocity = 10f
			};
			MyPlane.Region = tmpRegion;
		}
	}

	public override void HitCheckAll()
	{
	}

	public override void Show()
	{
	}
}
