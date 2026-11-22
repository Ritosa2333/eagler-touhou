using System.Drawing;

namespace Shooting;

public class Spell_Reimu : BaseSpell
{
	public Spell_Reimu(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Position = MyPlane.Position;
		Damage = 40;
		Region = 64;
		LifeTime = 260;
		Scale = 0f;
		new BackgroundBlack(StageData, Position);
		new EmitterGiveOutEnegy3D(StageData, OriginalPosition, Color.FromArgb(255, 100, 100));
		StageData.SoundPlay("se_cat00.wav");
		StageData.SoundPlay("se_nep00.wav");
		TxtureObject = TextureObjectDictionary["YINYANGYU"];
		Velocity = 2.2f;
		DirectionDegree = -90.0;
		AngularVelocityDegree = -3f;
		Accelerate = (0f - Velocity) / 210f;
		SpellList.Add(this);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (1 < Time && Time < 50)
		{
			ScaleVelocity += 0.0004f;
		}
		else if (150 < Time && Time < 240)
		{
			ScaleVelocity -= 0.0004f;
		}
		else if (Time == 240)
		{
			ScaleVelocity = (0f - Scale) / 20f;
		}
		else if (Time == 260)
		{
			ScaleVelocity = 0f;
		}
		if (Time == 22)
		{
			StageData.VibrateStart(LifeTime - Time);
		}
		if (Time == 260)
		{
			new EmitterGiveOutEnegy3D(StageData, OriginalPosition, Color.FromArgb(150, 150, 255));
			new EmitterGiveOutEnegy3D(StageData, OriginalPosition, Color.FromArgb(150, 150, 255));
			new EmitterGiveOutEnegy3D(StageData, OriginalPosition, Color.FromArgb(150, 150, 255));
			StageData.SoundPlay("se_cat00.wav");
		}
		if (Time % 12 == 0)
		{
			int num = 60;
			double num2 = 360f / (float)num;
			float num3 = ((Time < 150) ? (2f + (float)Time * 4f / 150f) : 6f);
			for (int i = 0; i < num; i++)
			{
				BaseEffect baseEffect = new BaseEffect(StageData, "yuan_9", Position, num3, 0.0)
				{
					DirectionDegree = num2 * (double)i,
					ScaleWidth = 0.4f,
					ScaleLength = 0.13f,
					LifeTime = 60,
					TransparentValueF = 0f,
					ColorValue = Color.FromArgb(100, 100, 200)
				};
				baseEffect.AccelerateDictionary.Add(1, (0f - (num3 - 0.5f)) / 50f);
				baseEffect.AccelerateDictionary.Add(50, 0f);
				baseEffect.ScaleWidthVelocityDictionary.Add(1, 0.01f);
				baseEffect.ScaleWidthVelocityDictionary.Add(50, 0f);
				baseEffect.ScaleLengthVelocityDictionary.Add(1, (0f - baseEffect.ScaleLength) / 50f);
				baseEffect.ScaleLengthVelocityDictionary.Add(50, 0f);
				baseEffect.TransparentVelocityDictionary.Add(1, 1f);
				baseEffect.TransparentVelocityDictionary.Add(60, 0f);
			}
		}
	}

	public override void Show()
	{
		base.Show();
		SpriteMain.Draw2D(TextureObjectDictionary["bullet50_3"], ScaleWidth * 0.13f / 0.04f, ScaleLength * 0.13f / 0.04f, 0f, Position, Color.FromArgb(76, ColorValue), Mirrored);
	}
}
