using System;
using System.Collections.Generic;
using System.Drawing;
using SlimDX;

namespace Shooting;

public class BaseMyPlane : BaseObject
{
	private int life = 2;

	private int spell = 2;

	private int lifeChip;

	private int spellChip;

	public readonly int nextSpellChip = 4;

	public readonly int[] nextLifeChip = new int[9] { 3, 5, 8, 10, 12, 15, 18, 20, 22 };

	private int lifeUpCount;

	private int power = 100;

	private long score;

	private long hiScore;

	private bool lastX;

	private const int unmatchedTime = 300;

	private const int deltaGreen = 120;

	private int starPoint;

	public readonly int maxStarPoint = 3000;

	private int nextBluePoint = 300;

	private int nextGreenPoint = 3000;

	private int bluePoint;

	private int greenPoint;

	private int blueLevel;

	private int greenLevel;

	private EnchantmentType enchantmentState;

	private EnchantmentType lastColor = EnchantmentType.Green;

	private int enchantmentTime;

	private float rate = 1f;

	private int enchantmentCount;

	private int enchantmentCountNeeded;

	private bool OnRotate;

	private int Angle3D;

	public string Name { get; set; }

	public string WeaponType { get; set; }

	public string FullName => Name + WeaponType;

	public bool SlowMove { get; private set; }

	public float HighSpeed { get; set; }

	public float LowSpeed { get; set; }

	public new float Vx { get; private set; }

	public new float Vy { get; private set; }

	public int DeadTime { get; set; }

	public int Life
	{
		get
		{
			return life;
		}
		set
		{
			if (value > 8)
			{
				life = 8;
			}
			else
			{
				life = value;
			}
		}
	}

	public int Spell
	{
		get
		{
			return spell;
		}
		set
		{
			if (value > 8)
			{
				spell = 8;
			}
			else if (value < 0)
			{
				spell = 0;
			}
			else
			{
				spell = value;
			}
		}
	}

	public int LifeChip
	{
		get
		{
			return lifeChip;
		}
		set
		{
			lifeChip = value;
			while (lifeChip >= nextLifeChip[lifeUpCount])
			{
				Extend();
				lifeChip -= nextLifeChip[lifeUpCount];
				lifeUpCount++;
			}
		}
	}

	public int SpellChip
	{
		get
		{
			return spellChip;
		}
		set
		{
			for (spellChip = value; spellChip >= nextSpellChip; spellChip -= nextSpellChip)
			{
				SpellExtand();
			}
		}
	}

	public int Power
	{
		get
		{
			return power;
		}
		set
		{
			if (value > 400)
			{
				power = 400;
			}
			else if (value < 100)
			{
				power = 100;
			}
			else
			{
				power = value;
			}
		}
	}

	public int PowerLevel => power / 100;

	public long Score
	{
		get
		{
			return score;
		}
		set
		{
			if (value > 99999999999L)
			{
				score = 99999999999L;
			}
			else if (value < 0)
			{
				score = 0L;
			}
			else
			{
				score = value;
			}
		}
	}

	public long HiScore
	{
		get
		{
			if (score > hiScore)
			{
				return score;
			}
			return hiScore;
		}
		set
		{
			if (value > 99999999999L)
			{
				hiScore = 99999999999L;
			}
			else if (value < 0)
			{
				hiScore = 0L;
			}
			else
			{
				hiScore = value;
			}
		}
	}

	public int BluePoint
	{
		get
		{
			return bluePoint;
		}
		set
		{
			bluePoint = value;
		}
	}

	public int GreenPoint
	{
		get
		{
			return greenPoint;
		}
		set
		{
			greenPoint = value;
		}
	}

	public int NextBluePoint
	{
		get
		{
			return nextBluePoint;
		}
		set
		{
			nextBluePoint = value;
		}
	}

	public int NextGreenPoint
	{
		get
		{
			return nextGreenPoint;
		}
		set
		{
			nextGreenPoint = value;
		}
	}

	public int StarPoint
	{
		get
		{
			return starPoint;
		}
		set
		{
			if (value >= maxStarPoint)
			{
				if (starPoint < maxStarPoint)
				{
					if (LastColor == EnchantmentType.Red)
					{
						ChangeEnchantment(EnchantmentType.Red);
					}
					else if (LastColor == EnchantmentType.Blue)
					{
						ChangeEnchantment(EnchantmentType.Blue);
					}
					else if (LastColor == EnchantmentType.Green)
					{
						ChangeEnchantment(EnchantmentType.Green);
					}
					StageData.SoundPlay("se_focusrot.wav");
					new BaseBackground(StageData, "pl02b", Position, 0f, 0.0)
					{
						ScaleVelocity = 1f,
						LifeTime = 26,
						TransparentVelocity = -10f,
						Active = false,
						ColorValue = Color.Black
					};
					new BaseBackground(StageData, "pl02b", Position, 0f, 0.0)
					{
						ScaleVelocity = 1f,
						LifeTime = 26,
						TransparentVelocity = -10f,
						Active = false,
						ColorValue = Color.Black
					};
					new BaseBackground(StageData, "pl02b", Position, 0f, 0.0)
					{
						ScaleVelocity = 1f,
						LifeTime = 26,
						TransparentVelocity = -10f,
						Active = false,
						ColorValue = Color.Black
					};
					new EnchantmentMagicCircle(StageData, OriginalPosition);
				}
				starPoint = maxStarPoint;
			}
			else if (value < 0)
			{
				starPoint = 0;
			}
			else
			{
				starPoint = value;
			}
		}
	}

	public EnchantmentType EnchantmentState
	{
		get
		{
			return enchantmentState;
		}
		set
		{
			enchantmentState = value;
		}
	}

	public EnchantmentType LastColor
	{
		get
		{
			return lastColor;
		}
		set
		{
			lastColor = value;
		}
	}

	public int EnchantmentTime
	{
		get
		{
			return enchantmentTime;
		}
		set
		{
			//IL_025c: Unknown result type (might be due to invalid IL or missing references)
			//IL_0271: Unknown result type (might be due to invalid IL or missing references)
			//IL_0286: Unknown result type (might be due to invalid IL or missing references)
			//IL_0402: Unknown result type (might be due to invalid IL or missing references)
			//IL_0417: Unknown result type (might be due to invalid IL or missing references)
			//IL_042c: Unknown result type (might be due to invalid IL or missing references)
			//IL_035d: Unknown result type (might be due to invalid IL or missing references)
			//IL_0517: Unknown result type (might be due to invalid IL or missing references)
			if (value > 0)
			{
				enchantmentTime = value;
				return;
			}
			if (enchantmentTime > 0)
			{
				if (enchantmentTime == 1)
				{
					EnchantmentCount++;
					Rate += 0.1f;
					long num = 0L;
					if (enchantmentState == EnchantmentType.Blue)
					{
						num = HighItemScore * 20;
					}
					else if (enchantmentState == EnchantmentType.Green)
					{
						num = 1000000L;
					}
					else if (enchantmentState == EnchantmentType.Red)
					{
						num = 5120000L;
					}
					score += num;
					new EnchantmentBonus(StageData, num);
					StageData.SoundPlay("se_cardget.wav");
				}
				else
				{
					new BaseEffect(StageData, "pl02b", Position, 0f, 0.0)
					{
						ScaleVelocity = 1f,
						LifeTime = 26,
						TransparentVelocity = -10f,
						Active = false,
						ColorValue = Color.Black
					};
					new BaseEffect(StageData, "pl02b", Position, 0f, 0.0)
					{
						ScaleVelocity = 1f,
						LifeTime = 26,
						TransparentVelocity = -10f,
						Active = false,
						ColorValue = Color.Black
					};
					new BaseEffect(StageData, "pl02b", Position, 0f, 0.0)
					{
						ScaleVelocity = 1f,
						LifeTime = 26,
						TransparentVelocity = -10f,
						Active = false,
						ColorValue = Color.Black
					};
				}
				if (enchantmentState == EnchantmentType.Green)
				{
					Time = UnmatchedTime - 60;
					DeadTime = -1;
					new SpellDisable(StageData);
				}
				StarPoint = 0;
				StageData.SoundPlay("ice005.wav");
				TextureObject txtureObject = TextureObjectDictionary["White"];
				int num2 = 16;
				int num3 = 8;
				Color color = Color.White;
				if (enchantmentState == EnchantmentType.Blue)
				{
					color = Color.Blue;
				}
				else if (enchantmentState == EnchantmentType.Green)
				{
					color = Color.Green;
				}
				else if (enchantmentState == EnchantmentType.Red)
				{
					color = Color.Red;
				}
				Model_Triangle model = new Model_Triangle(StageData.DeviceMain, txtureObject, "", new Vector2(0f, 0f), new Vector2(1f / (float)num2, 0f / (float)num3), new Vector2(0f / (float)num2, 1f / (float)num3), 640 / num2, 480 / num3);
				for (int i = 0; i < num2; i++)
				{
					for (int j = 0; j < num3; j++)
					{
						BaseParticle3D baseParticle3D = new BaseParticle3D();
						baseParticle3D.StageData = StageData;
						baseParticle3D.Position = new PointF(i * 640 / num2, j * 480 / num3);
						baseParticle3D.model = model;
						baseParticle3D.Velocity = (float)Ran.Next(20) / 10f;
						baseParticle3D.LifeTime = 30;
						baseParticle3D.DirectionDegree = Ran.Next(360);
						baseParticle3D.RotatingAxis = new Vector3((float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100));
						baseParticle3D.AngularVelocity3D = (float)Ran.Next(20) / 100f;
						baseParticle3D.TransparentVelocity = -6f;
						baseParticle3D.ScaleVelocity = -0.005f;
						baseParticle3D.ColorValue = color;
						baseParticle3D.TransparentValueF = 150f;
						StageData.Particle3D.ParticleList.Add(baseParticle3D);
					}
				}
				model = new Model_Triangle(StageData.DeviceMain, txtureObject, "", new Vector2(1f / (float)num2, 1f / (float)num3), new Vector2(0f / (float)num2, 1f / (float)num3), new Vector2(1f / (float)num2, 0f / (float)num3), 640 / num2, 480 / num3);
				for (int k = 0; k < num2; k++)
				{
					for (int l = 0; l < num3; l++)
					{
						BaseParticle3D baseParticle3D2 = new BaseParticle3D();
						baseParticle3D2.StageData = StageData;
						baseParticle3D2.Position = new PointF((k + 1) * 640 / num2, (l + 1) * 480 / num3);
						baseParticle3D2.model = model;
						baseParticle3D2.Angle = Math.PI;
						baseParticle3D2.Velocity = (float)Ran.Next(20) / 10f;
						baseParticle3D2.LifeTime = 30;
						baseParticle3D2.DirectionDegree = Ran.Next(360);
						baseParticle3D2.RotatingAxis = new Vector3((float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100));
						baseParticle3D2.AngularVelocity3D = (float)Ran.Next(20) / 100f;
						baseParticle3D2.TransparentVelocity = -6f;
						baseParticle3D2.ScaleVelocity = -0.005f;
						baseParticle3D2.ColorValue = color;
						baseParticle3D2.TransparentValueF = 150f;
						StageData.Particle3D.ParticleList.Add(baseParticle3D2);
					}
				}
			}
			enchantmentTime = 0;
			enchantmentState = EnchantmentType.None;
		}
	}

	public int EnchantmentCount
	{
		get
		{
			return enchantmentCount;
		}
		set
		{
			enchantmentCount = value;
		}
	}

	public int EnchantmentCountNeeded
	{
		get
		{
			return enchantmentCountNeeded;
		}
		set
		{
			enchantmentCountNeeded = value;
		}
	}

	public bool FscEnabled
	{
		get
		{
			if (EnchantmentCount >= EnchantmentCountNeeded)
			{
				return Difficulty > DifficultLevel.Normal;
			}
			return false;
		}
	}

	public float Rate
	{
		get
		{
			return rate;
		}
		set
		{
			rate = value;
		}
	}

	public int HighItemScore { get; set; }

	public int Graze { get; set; }

	public int UnmatchedTime => 300;

	public TextureObject[] TxtureObjects { get; set; }

	public PointF[] SubPlanePoint { get; set; }

	public List<BaseSubPlane> SubPlaneList { get; set; }

	public bool SpellEnabled { get; set; }

	public int LifeUpCount
	{
		get
		{
			return lifeUpCount;
		}
		set
		{
			lifeUpCount = value;
		}
	}

	public BaseMyPlane()
	{
	}

	public BaseMyPlane(StageDataPackage StageData, Point Position)
	{
		base.StageData = StageData;
		base.Position = Position;
		TxtureObjects = new TextureObject[72];
		for (int i = 0; i < 72; i++)
		{
			TxtureObjects[i] = TextureObjectDictionary["MyPlane" + i.ToString("D3")];
		}
		TxtureObject = TxtureObjects[0];
		SubPlanePoint = new PointF[4];
		HighSpeed = 4.5f;
		LowSpeed = 2f;
		Region = 1;
		Time = UnmatchedTime;
		DeadTime = -1;
		SubPlaneList = new List<BaseSubPlane>
		{
			new BaseSubPlane(StageData, "MySubPlane", Position),
			new BaseSubPlane(StageData, "MySubPlane", Position),
			new BaseSubPlane(StageData, "MySubPlane", Position),
			new BaseSubPlane(StageData, "MySubPlane", Position)
		};
		Name = "Plane";
		WeaponType = "A";
		SpellEnabled = true;
		HighItemScore = 100000;
	}

	public virtual void Refresh()
	{
		Time = 300;
		EnchantmentCount = 0;
		SubPlaneList.ForEach((BaseSubPlane x) =>
		{
			x.Refresh();
		});
		SpellEnabled = true;
	}

	public override void Move()
	{
		Vx = 0f;
		Vy = 0f;
		float x = OriginalPosition.X;
		float y = OriginalPosition.Y;
		SlowMove = KClass.Key_Shift;
		if (!SlowMove)
		{
			if (KClass.ArrowRight)
			{
				Vx += HighSpeed;
			}
			if (KClass.ArrowLeft)
			{
				Vx -= HighSpeed;
			}
			if (KClass.ArrowUp)
			{
				Vy += HighSpeed;
			}
			if (KClass.ArrowDown)
			{
				Vy -= HighSpeed;
			}
		}
		else
		{
			if (KClass.ArrowRight)
			{
				Vx += LowSpeed;
			}
			if (KClass.ArrowLeft)
			{
				Vx -= LowSpeed;
			}
			if (KClass.ArrowUp)
			{
				Vy += LowSpeed;
			}
			if (KClass.ArrowDown)
			{
				Vy -= LowSpeed;
			}
		}
		if (Vx != 0f && Vy != 0f)
		{
			Vx = 0.71f * Vx;
			Vy = 0.71f * Vy;
		}
		x += Vx;
		y -= Vy;
		if (x < (float)(TxtureObject.Width / 2 - 6))
		{
			x = TxtureObject.Width / 2 - 6;
		}
		else if (x > (float)(BoundRect.Width - TxtureObject.Width / 2 + 6))
		{
			x = BoundRect.Width - TxtureObject.Width / 2 + 6;
		}
		if (y < (float)(TxtureObject.Height / 2))
		{
			y = TxtureObject.Height / 2;
		}
		else if (y >= (float)(BoundRect.Height - TxtureObject.Height / 2 + 8))
		{
			y = BoundRect.Height - TxtureObject.Height / 2 + 8;
		}
		OriginalPosition = new PointF(x, y);
	}

	public virtual void SubPlaneCtrl()
	{
		if (KClass.Key_Shift)
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 30f);
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 15f, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 15f, OriginalPosition.Y - 30f);
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 30f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 20f, OriginalPosition.Y - 20f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 20f, OriginalPosition.Y - 20f);
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 15f, OriginalPosition.Y - 40f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 15f, OriginalPosition.Y - 40f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 30f, OriginalPosition.Y - 30f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 30f, OriginalPosition.Y - 30f);
				break;
			}
		}
		else
		{
			switch (PowerLevel)
			{
			case 1:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 40f);
				break;
			case 2:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y - 20f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y - 20f);
				break;
			case 3:
				SubPlanePoint[0] = new PointF(OriginalPosition.X, OriginalPosition.Y - 40f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 30f, OriginalPosition.Y - 20f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 30f, OriginalPosition.Y - 20f);
				break;
			case 4:
				SubPlanePoint[0] = new PointF(OriginalPosition.X - 25f, OriginalPosition.Y - 0f);
				SubPlanePoint[1] = new PointF(OriginalPosition.X + 25f, OriginalPosition.Y - 0f);
				SubPlanePoint[2] = new PointF(OriginalPosition.X - 50f, OriginalPosition.Y + 5f);
				SubPlanePoint[3] = new PointF(OriginalPosition.X + 50f, OriginalPosition.Y + 5f);
				break;
			}
		}
		for (int i = 0; i < PowerLevel; i++)
		{
			SubPlaneList[i].Enabled = true;
			SubPlaneList[i].DestPoint = SubPlanePoint[i];
			SubPlaneList[i].Ctrl();
		}
		for (int j = PowerLevel; j < SubPlaneList.Count; j++)
		{
			SubPlaneList[j].Enabled = false;
			SubPlaneList[j].Position = Position;
		}
	}

	public virtual void TextureCtrl()
	{
		if (Vx > 0.5f)
		{
			if (SlowMove)
			{
				OnRotate = false;
				if (0 <= Angle3D && Angle3D < 30)
				{
					Angle3D += 5;
				}
				else if (Angle3D < 0)
				{
					Angle3D = 0;
				}
			}
			else if (!OnRotate)
			{
				if (0 <= Angle3D && Angle3D < 40)
				{
					Angle3D += 5;
				}
				else if (Angle3D < 0)
				{
					OnRotate = true;
				}
			}
			else
			{
				Angle3D += 13;
			}
		}
		else if (Vx < -0.5f)
		{
			if (SlowMove)
			{
				OnRotate = false;
				if (-30 < Angle3D && Angle3D <= 0)
				{
					Angle3D -= 5;
				}
				else if (Angle3D > 0)
				{
					Angle3D = 0;
				}
			}
			else if (!OnRotate)
			{
				if (-40 < Angle3D && Angle3D <= 0)
				{
					Angle3D -= 5;
				}
				else if (Angle3D > 0)
				{
					OnRotate = true;
				}
			}
			else
			{
				Angle3D -= 13;
			}
		}
		else
		{
			Angle3D /= 2;
			if (-10 < Angle3D && Angle3D < 10)
			{
				Angle3D = 0;
				OnRotate = false;
			}
		}
		if (Angle3D > 180)
		{
			Angle3D -= 360;
		}
		else if (Angle3D < -180)
		{
			Angle3D += 360;
		}
		int num = Angle3D / 5;
		if (num < 0)
		{
			num += 72;
		}
		else if (num > 72)
		{
			num -= 72;
		}
		TxtureObject = TxtureObjects[num];
	}

	public override void Shoot()
	{
		if (Time % 3 == 0)
		{
			StageData.SoundPlay("se_plst00.wav", OriginalPosition.X / (float)BoundRect.Width);
			float x = Position.X;
			float y = Position.Y;
			float num = 7f;
			num *= (float)Math.Cos((double)Angle3D * Math.PI / 180.0);
			new BaseMyBullet(StageData, "自机子弹", new PointF(x - num, y), 30f, -Math.PI / 2.0);
			new BaseMyBullet(StageData, "自机子弹", new PointF(x + num, y), 30f, -Math.PI / 2.0);
		}
		if (Time % 5 == 0)
		{
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Shoot();
			}
		}
	}

	public virtual void SpellShoot()
	{
		new Spell(StageData);
		StageData.SoundPlay("se_nep00.wav");
	}

	public virtual void PreMiss()
	{
		if (Time < UnmatchedTime || Time < DeadTime || SpellList.Count != 0)
		{
			return;
		}
		if (EnchantmentState == EnchantmentType.Green)
		{
			new BulletRemover_Small(StageData, OriginalPosition).Region = 500;
			ChangeEnchantment(EnchantmentType.None);
			EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
			{
				if (x is BaseSpellCard)
				{
					((BaseSpellCard)x).Missed = true;
				}
			});
		}
		else
		{
			new Emitter(StageData, MyPlane.Position);
			StageData.SoundPlay("se_pldead00.wav", OriginalPosition.X / (float)BoundRect.Width);
			DeadTime = Time + 20;
		}
	}

	public virtual void Miss()
	{
		if (EnchantmentState == EnchantmentType.Red || EnchantmentState == EnchantmentType.Blue)
		{
			ChangeEnchantment(EnchantmentType.None);
		}
		new BulletRemover(StageData, OriginalPosition);
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height + 100);
		Vx = 0f;
		Vy = 0f;
		Life--;
		Power -= 16;
		Time = 0;
		DeadTime = -1;
	}

	public override void Ctrl()
	{
		if (!Enabled)
		{
			return;
		}
		Time++;
		if (Time == DeadTime)
		{
			Miss();
		}
		else if (Time > DeadTime)
		{
			if (Time == 1 && Spell < 2)
			{
				Spell = 2;
			}
			if (Time > 70)
			{
				Move();
			}
			else
			{
				DestPoint = new PointF(BoundRect.Width / 2, BoundRect.Height - 30);
				Velocity = 2f;
				base.Move();
				TextureCtrl();
			}
			TextureCtrl();
			Direction += 0.019999999552965164;
			SubPlaneCtrl();
			EnchantmentTime--;
			if (EnchantmentState == EnchantmentType.Blue)
			{
				ItemList.ForEach((BaseItem x) =>
				{
					if (!(x is ShootingStarShard))
					{
						x.Obtain = true;
						x.Doubled = true;
					}
				});
			}
		}
		if (Story != null || Time <= 70)
		{
			return;
		}
		if (KClass.Key_Z && TimeMain > 15)
		{
			Shoot();
		}
		if (KClass.Key_X && !lastX)
		{
			if (EnchantmentState == EnchantmentType.Green)
			{
				new BulletRemover_Small(StageData, OriginalPosition).Region = 500;
				ChangeEnchantment(EnchantmentType.None);
				EnemyPlaneList.ForEach((BaseEnemyPlane x) =>
				{
					if (x is BaseSpellCard)
					{
						((BaseSpellCard)x).Bombed = true;
					}
				});
			}
			else if (Spell > 0 && SpellList.Count == 0 && Time > 30 && SpellEnabled)
			{
				SpellShoot();
				Spell--;
			}
		}
		lastX = KClass.Key_X;
	}

	public void RetryClear()
	{
		EnchantmentTime = 0;
		Life = 2;
		Spell = 2;
		Power = 400;
		Score = StageData.ContinueTimes;
		Graze = 0;
		HiScore = StageData.S_History[0].Score;
		LifeChip = 0;
		SpellChip = 0;
		LifeUpCount = 0;
		StarPoint = 0;
		HighItemScore = 100000;
		Rate = 0f;
		LastColor = EnchantmentType.Green;
	}

	public void Extend()
	{
		if (Life < 8)
		{
			Life++;
		}
		else
		{
			Spell++;
		}
		StageData.SoundPlay("se_extend.wav");
		new BaseEffect(StageData, "Extend", new PointF(0f, 0f), 0f, Math.PI / 2.0)
		{
			Active = false,
			OriginalPosition = new PointF(BoundRect.Width / 2, 112f),
			LifeTime = 90,
			TransparentValueF = 0f,
			Layer = 1,
			TransparentVelocityDictionary = 
			{
				{ 1, 13f },
				{ 70, -13f }
			}
		};
	}

	public void SpellExtand()
	{
		StageData.SoundPlay("se_cardget.wav");
		Spell++;
		new BaseEffect(StageData, "SpellExtend", new PointF(0f, 0f), 0f, Math.PI / 2.0)
		{
			Active = false,
			OriginalPosition = new PointF(BoundRect.Width / 2, 80f),
			LifeTime = 90,
			TransparentValueF = 0f,
			Layer = 1,
			TransparentVelocityDictionary = 
			{
				{ 1, 13f },
				{ 70, -13f }
			}
		};
	}

	public void ChangeEnchantment(EnchantmentType EnchantmentState)
	{
		switch (EnchantmentState)
		{
		case EnchantmentType.None:
			EnchantmentTime = 0;
			break;
		case EnchantmentType.Red:
			this.EnchantmentState = EnchantmentState;
			EnchantmentTime = 600;
			break;
		case EnchantmentType.Blue:
			this.EnchantmentState = EnchantmentState;
			EnchantmentTime = 600;
			break;
		case EnchantmentType.Green:
			this.EnchantmentState = EnchantmentState;
			EnchantmentTime = 420;
			break;
		default:
			EnchantmentTime = 0;
			break;
		}
	}

	public override void Show()
	{
		if (Time >= DeadTime && Time != 0)
		{
			if (Time < UnmatchedTime && Time / 2 % 2 == 0)
			{
				SpriteMain.Draw2D(TxtureObject, 1f, 0f, Position, Color.DarkBlue);
			}
			else
			{
				SpriteMain.Draw2D(TxtureObject, 1f, 0f, Position, Color.White);
			}
			for (int i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Show();
			}
		}
	}

	public virtual void ShowCenter()
	{
		if (SlowMove)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["MyPlaneCenter1"], 1f, (float)Direction * 4f, Position, Color.White);
		}
	}
}
