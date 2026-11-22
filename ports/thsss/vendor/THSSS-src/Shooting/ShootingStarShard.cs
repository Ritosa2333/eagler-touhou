using System;
using System.Drawing;

namespace Shooting;

internal class ShootingStarShard : BaseItem_Touhou
{
	public ShootingStarShard(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, "StarShardBS", OriginalPosition)
	{
		Region = 8;
		AngularVelocity = 0f;
		DirectionDegree += (float)Ran.Next(-49, 50) / 10f;
		TransparentValueF = 0f;
		TransparentVelocity = 16f;
		Active = true;
		ColorValue = Color.Blue;
	}

	public override void HitCheckAll()
	{
		if (HitCheck(MyPlane, 48f))
		{
			Obtain = true;
		}
		if (HitCheck(MyPlane))
		{
			ItemList.Remove(this);
			if (ColorValue == Color.Red)
			{
				MyPlane.LastColor = EnchantmentType.Red;
			}
			else if (ColorValue == Color.Blue)
			{
				MyPlane.LastColor = EnchantmentType.Blue;
			}
			else if (ColorValue == Color.Green)
			{
				MyPlane.LastColor = EnchantmentType.Green;
			}
			MyPlane.StarPoint += 800;
			if (ColorValue == Color.Red)
			{
				MyPlane.LifeChip++;
			}
			else if (ColorValue == Color.Blue)
			{
				MyPlane.HighItemScore += 5000;
			}
			else if (ColorValue == Color.Green)
			{
				MyPlane.SpellChip++;
			}
			StageData.SoundPlay("se_item00.wav", OriginalPosition.X / (float)BoundRect.Width);
			for (int i = 0; i < 50; i++)
			{
				string textureName = "光点";
				new ParticleSmaller(StageData, textureName, Position, Ran.Next(20, 200) / 10, (double)((float)Ran.Next(360) / 180f) * Math.PI)
				{
					LifeTime = 20,
					Scale = (float)Ran.Next(100) / 100f,
					TransparentValueF = 200f,
					ColorValue = ColorValue
				};
			}
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 200)
		{
			ColorValue = Color.Red;
			TxtureObject = TextureObjectDictionary["StarShardRS"];
		}
		else if (Time == 300)
		{
			ColorValue = Color.Green;
			TxtureObject = TextureObjectDictionary["StarShardGS"];
		}
		GhostingColor = ColorValue;
		if (Time > 40)
		{
			AngularVelocityDegree = 3f;
			AngleDegree += AngularVelocityDegree;
		}
		if (Time % 4 == 0)
		{
			new BaseEffect(StageData, "StarShardW", Position, 0f, 0.0)
			{
				Angle = Angle,
				ScaleVelocity = -0.0125f,
				TransparentVelocity = -3f,
				LifeTime = 80,
				ColorValue = ColorValue
			};
		}
	}

	public override void Show()
	{
		Color color = ColorValue;
		ColorValue = Color.White;
		base.Show();
		ColorValue = color;
	}
}
