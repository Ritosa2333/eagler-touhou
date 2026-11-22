using System;
using System.Drawing;

namespace Shooting;

public class Spell_Shinmyoumaru : BaseSpell
{
	public Spell_Shinmyoumaru(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Position = MyPlane.Position;
		Damage = 32;
		Region = 4;
		LifeTime = 300;
		ScaleVelocity = 0.3f;
		MaxScale = 200f;
		new BackgroundBlack(StageData, Position);
		StageData.SoundPlay("se_cat00.wav");
		StageData.SoundPlay("se_nep00.wav");
		SpellList.Add(this);
	}

	public override void Move()
	{
		Position = MyPlane.Position;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (1 < Time && Time < 60)
		{
			int num = 7;
			double num2 = 360f / (float)num;
			for (int i = 0; i < num; i++)
			{
				BaseEffect baseEffect = new BaseEffect(StageData, "bullet23_0", Position, 1.5f, 0.0);
				baseEffect.DirectionDegree = (double)(Time * 18) + num2 * (double)i;
				baseEffect.ScaleWidth = 0.3f;
				baseEffect.ScaleLength = 0.3f;
				baseEffect.LifeTime = 999;
				baseEffect.OutBound = false;
				baseEffect.DirectionVelocityDegree = -2.8f;
				baseEffect.VelocityDictionary.Add(40, 3f);
				baseEffect.AccelerateDictionary.Add(115, 2f);
				baseEffect.AccelerateDictionary.Add(123, 0f);
				baseEffect.DirectionVelocityDegreeDictionary.Add(115, (-80f + (float)Ran.NextPMDouble()) / 45f);
				baseEffect.DirectionVelocityDegreeDictionary.Add(160, 0f);
				baseEffect.ScaleLengthVelocityDictionary.Add(115, 0.3f);
				baseEffect.ScaleWidthVelocityDictionary.Add(115, 0.1f);
				baseEffect.ScaleLengthVelocityDictionary.Add(120, 0f);
				baseEffect.ScaleWidthVelocityDictionary.Add(120, 0f);
				baseEffect.SetBinding(this);
				BaseEffect baseEffect2 = new BaseEffect(StageData, "bullet23_0", Position, 1.5f, 0.0);
				baseEffect2.DirectionDegree = (double)(180 - Time * 18) + num2 * (double)i;
				baseEffect2.ScaleWidth = 0.3f;
				baseEffect2.ScaleLength = 0.3f;
				baseEffect2.LifeTime = 999;
				baseEffect2.OutBound = false;
				baseEffect2.DirectionVelocityDegree = 2.8f;
				baseEffect2.VelocityDictionary.Add(40, 3f);
				baseEffect2.AccelerateDictionary.Add(115, 2f);
				baseEffect2.AccelerateDictionary.Add(123, 0f);
				baseEffect2.DirectionVelocityDegreeDictionary.Add(115, (80f + (float)Ran.NextPMDouble()) / 45f);
				baseEffect2.DirectionVelocityDegreeDictionary.Add(160, 0f);
				baseEffect2.ScaleLengthVelocityDictionary.Add(115, 0.3f);
				baseEffect2.ScaleWidthVelocityDictionary.Add(115, 0.1f);
				baseEffect2.ScaleLengthVelocityDictionary.Add(120, 0f);
				baseEffect2.ScaleWidthVelocityDictionary.Add(120, 0f);
				baseEffect2.SetBinding(this);
			}
		}
		if (130 < Time && Time < 220)
		{
			for (int j = 0; j < 3; j++)
			{
				BaseEffect_CS baseEffect_CS = new BaseEffect_CS(StageData);
				baseEffect_CS.TxtureObject = TextureObjectDictionary["bullet50_0"];
				baseEffect_CS.OriginalPosition = new PointF((float)(BoundRect.Width / 2) + 200f * (float)Ran.NextPMDouble(), (float)(BoundRect.Height / 2) + 280f * (float)Ran.NextPMDouble());
				baseEffect_CS.Velocity = (float)Ran.NextPMDouble();
				baseEffect_CS.Direction = Math.PI / 2.0 * Ran.NextPMDouble();
				baseEffect_CS.Scale = 0.05f;
				baseEffect_CS.LifeTime = 40;
				EventGroup eventGroup = new EventGroup();
				eventGroup.index = 0;
				eventGroup.tag = "0";
				eventGroup.t = 15;
				eventGroup.addtime = 15;
				Event obj = new Event();
				obj.EventString = "当前帧=1：高比变化到0，正弦，14";
				obj.String2BulletEvent();
				eventGroup.EventList.Add(obj);
				obj = new Event();
				obj.EventString = "当前帧=1：宽比变化到0，正弦，14";
				obj.String2BulletEvent();
				eventGroup.EventList.Add(obj);
				baseEffect_CS.EventGroupList.Add(eventGroup);
				EffectList.Add(baseEffect_CS);
			}
		}
		if (Time == 122)
		{
			ScaleVelocity = 10f;
			new EmitterGiveOutEnegy3D(StageData, OriginalPosition, Color.White);
			StageData.SoundPlay("se_cat00.wav");
			StageData.VibrateStart(LifeTime - Time);
		}
	}
}
