using System;
using System.Collections.Generic;
using System.Drawing;
using SlimDX;

namespace Shooting
{
	public class BaseMyPlane : BaseObject
	{
		public string Name { get; set; }

		public string WeaponType { get; set; }

		public string FullName
		{
			get
			{
				return Name + WeaponType;
			}
		}

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
				spellChip = value;
				while (spellChip >= nextSpellChip)
				{
					SpellExtand();
					spellChip -= nextSpellChip;
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

		public int PowerLevel
		{
			get
			{
				return power / 100;
			}
		}

		public long Score
		{
			get
			{
				return score;
			}
			set
			{
				if (value > 9999999999L)
				{
					score = 9999999999L;
				}
				else if (value < 0L)
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
				long num;
				if (score > hiScore)
				{
					num = score;
				}
				else
				{
					num = hiScore;
				}
				return num;
			}
			set
			{
				if (value > 9999999999L)
				{
					hiScore = 9999999999L;
				}
				else if (value < 0L)
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
						var baseBackground = new BaseBackground(StageData, "pl02b", Position, 0f, 0.0);
						baseBackground.ScaleVelocity = 1f;
						baseBackground.LifeTime = 26;
						baseBackground.TransparentVelocity = -10f;
						baseBackground.Active = false;
						baseBackground.ColorValue = Color.Black;
						var baseBackground2 = new BaseBackground(StageData, "pl02b", Position, 0f, 0.0);
						baseBackground2.ScaleVelocity = 1f;
						baseBackground2.LifeTime = 26;
						baseBackground2.TransparentVelocity = -10f;
						baseBackground2.Active = false;
						baseBackground2.ColorValue = Color.Black;
						var baseBackground3 = new BaseBackground(StageData, "pl02b", Position, 0f, 0.0);
						baseBackground3.ScaleVelocity = 1f;
						baseBackground3.LifeTime = 26;
						baseBackground3.TransparentVelocity = -10f;
						baseBackground3.Active = false;
						baseBackground3.ColorValue = Color.Black;
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
				if (value > 0)
				{
					enchantmentTime = value;
				}
				else
				{
					if (enchantmentTime > 0)
					{
						if (enchantmentTime == 1)
						{
							EnchantmentCount++;
							Rate += 0.1f;
							var num = 0L;
							if (enchantmentState == EnchantmentType.Blue)
							{
								num = (long)(HighItemScore * 20);
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
							var baseEffect = new BaseEffect(StageData, "pl02b", Position, 0f, 0.0);
							baseEffect.ScaleVelocity = 1f;
							baseEffect.LifeTime = 26;
							baseEffect.TransparentVelocity = -10f;
							baseEffect.Active = false;
							baseEffect.ColorValue = Color.Black;
							var baseEffect2 = new BaseEffect(StageData, "pl02b", Position, 0f, 0.0);
							baseEffect2.ScaleVelocity = 1f;
							baseEffect2.LifeTime = 26;
							baseEffect2.TransparentVelocity = -10f;
							baseEffect2.Active = false;
							baseEffect2.ColorValue = Color.Black;
							var baseEffect3 = new BaseEffect(StageData, "pl02b", Position, 0f, 0.0);
							baseEffect3.ScaleVelocity = 1f;
							baseEffect3.LifeTime = 26;
							baseEffect3.TransparentVelocity = -10f;
							baseEffect3.Active = false;
							baseEffect3.ColorValue = Color.Black;
						}
						if (enchantmentState == EnchantmentType.Green)
						{
							Time = UnmatchedTime - 60;
							DeadTime = -1;
							new SpellDisable(StageData);
						}
						StarPoint = 0;
						StageData.SoundPlay("ice005.wav");
						var textureObject = TextureObjectDictionary["White"];
						var num2 = 16;
						var num3 = 8;
						var color = Color.White;
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
						for (var i = 0; i < num2; i++)
						{
							for (var j = 0; j < num3; j++)
							{
								var model_Triangle = new Model_Triangle(StageData.DeviceMain, textureObject, i.ToString() + "_" + j.ToString(), new Vector2((float)i / (float)num2, (float)j / (float)num3), new Vector2((float)(i + 1) / (float)num2, (float)j / (float)num3), new Vector2((float)i / (float)num2, (float)(j + 1) / (float)num3), 640 / num2, 480 / num3);
								var baseParticle3D = new BaseParticle3D();
								baseParticle3D.StageData = StageData;
								baseParticle3D.Position = new PointF((float)(i * 640 / num2), (float)(j * 480 / num3));
								baseParticle3D.model = model_Triangle;
								baseParticle3D.Velocity = (float)Ran.Next(20) / 10f;
								baseParticle3D.LifeTime = 30;
								baseParticle3D.DirectionDegree = (double)Ran.Next(360);
								baseParticle3D.RotatingAxis = new Vector3((float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100));
								baseParticle3D.AngularVelocity3D = (float)Ran.Next(20) / 100f;
								baseParticle3D.TransparentVelocity = -6f;
								baseParticle3D.ScaleVelocity = -0.005f;
								baseParticle3D.ColorValue = color;
								baseParticle3D.TransparentValueF = 150f;
								StageData.Particle3D.ParticleList.Add(baseParticle3D);
							}
						}
						for (var i = 0; i < num2; i++)
						{
							for (var j = 0; j < num3; j++)
							{
								var model_Triangle = new Model_Triangle(StageData.DeviceMain, textureObject, i.ToString() + "_" + j.ToString(), new Vector2((float)(i + 1) / (float)num2, (float)(j + 1) / (float)num3), new Vector2((float)i / (float)num2, (float)(j + 1) / (float)num3), new Vector2((float)(i + 1) / (float)num2, (float)j / (float)num3), 640 / num2, 480 / num3);
								var baseParticle3D = new BaseParticle3D();
								baseParticle3D.StageData = StageData;
								baseParticle3D.Position = new PointF((float)((i + 1) * 640 / num2), (float)((j + 1) * 480 / num3));
								baseParticle3D.model = model_Triangle;
								baseParticle3D.Angle = 3.141592653589793;
								baseParticle3D.Velocity = (float)Ran.Next(20) / 10f;
								baseParticle3D.LifeTime = 30;
								baseParticle3D.DirectionDegree = (double)Ran.Next(360);
								baseParticle3D.RotatingAxis = new Vector3((float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100), (float)Ran.Next(-100, 100));
								baseParticle3D.AngularVelocity3D = (float)Ran.Next(20) / 100f;
								baseParticle3D.TransparentVelocity = -6f;
								baseParticle3D.ScaleVelocity = -0.005f;
								baseParticle3D.ColorValue = color;
								baseParticle3D.TransparentValueF = 150f;
								StageData.Particle3D.ParticleList.Add(baseParticle3D);
							}
						}
					}
					enchantmentTime = 0;
					enchantmentState = EnchantmentType.None;
				}
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
				return EnchantmentCount >= EnchantmentCountNeeded && Difficulty > DifficultLevel.Normal;
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

		public int UnmatchedTime
		{
			get
			{
				return 300;
			}
		}

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
			this.StageData = StageData;
			this.Position = Position;
			TxtureObjects = new TextureObject[72];
			for (var i = 0; i < 72; i++)
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
			SubPlaneList.ForEach(delegate(BaseSubPlane x)
			{
				x.Refresh();
			});
			SpellEnabled = true;
		}

		public override void Move()
		{
			Vx = 0f;
			Vy = 0f;
			var num = OriginalPosition.X;
			var num2 = OriginalPosition.Y;
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
			num += Vx;
			num2 -= Vy;
			OriginalPosition = ClampPosition(num, num2);
		}

		public void ShiftBy(float dx, float dy)
		{
			if (TxtureObject == null)
			{
				return;
			}
			OriginalPosition = ClampPosition(OriginalPosition.X + dx, OriginalPosition.Y + dy);
		}

		private PointF ClampPosition(float num, float num2)
		{
			if (num < (float)(TxtureObject.Width / 2 - 6))
			{
				num = (float)(TxtureObject.Width / 2 - 6);
			}
			else if (num > (float)(BoundRect.Width - TxtureObject.Width / 2 + 6))
			{
				num = (float)(BoundRect.Width - TxtureObject.Width / 2 + 6);
			}
			if (num2 < (float)(TxtureObject.Height / 2))
			{
				num2 = (float)(TxtureObject.Height / 2);
			}
			else if (num2 >= (float)(BoundRect.Height - TxtureObject.Height / 2 + 8))
			{
				num2 = (float)(BoundRect.Height - TxtureObject.Height / 2 + 8);
			}
			return new PointF(num, num2);
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
			for (var i = 0; i < PowerLevel; i++)
			{
				SubPlaneList[i].Enabled = true;
				SubPlaneList[i].DestPoint = SubPlanePoint[i];
				SubPlaneList[i].Ctrl();
			}
			for (var i = PowerLevel; i < SubPlaneList.Count; i++)
			{
				SubPlaneList[i].Enabled = false;
				SubPlaneList[i].Position = Position;
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
			var num = Angle3D / 5;
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
				var x = Position.X;
				var y = Position.Y;
				var num = 7f;
				num *= (float)Math.Cos((double)Angle3D * 3.141592653589793 / 180.0);
				new BaseMyBullet(StageData, "自机子弹", new PointF(x - num, y), 30f, -1.5707963267948966);
				new BaseMyBullet(StageData, "自机子弹", new PointF(x + num, y), 30f, -1.5707963267948966);
			}
			if (Time % 5 == 0)
			{
				for (var i = 0; i < PowerLevel; i++)
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
			if (Time >= UnmatchedTime)
			{
				if (Time >= DeadTime)
				{
					if (SpellList.Count == 0)
					{
						if (EnchantmentState == EnchantmentType.Green)
						{
							var bulletRemover_Small = new BulletRemover_Small(StageData, OriginalPosition);
							bulletRemover_Small.Region = 500;
							ChangeEnchantment(EnchantmentType.None);
							EnemyPlaneList.ForEach(delegate(BaseEnemyPlane x)
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
				}
			}
		}

		public virtual void Miss()
		{
			if (EnchantmentState == EnchantmentType.Red || EnchantmentState == EnchantmentType.Blue)
			{
				ChangeEnchantment(EnchantmentType.None);
			}
			new BulletRemover(StageData, OriginalPosition);
			OriginalPosition = new PointF((float)(BoundRect.Width / 2), (float)(BoundRect.Height + 100));
			Vx = 0f;
			Vy = 0f;
			Life--;
			Power -= 16;
			Time = 0;
			DeadTime = -1;
		}

		public override void Ctrl()
		{
			if (Enabled)
			{
				Time++;
				if (Time == DeadTime)
				{
					Miss();
				}
				else if (Time > DeadTime)
				{
					if (Time == 1)
					{
						if (Spell < 2)
						{
							Spell = 2;
						}
					}
					if (Time > 70)
					{
						Move();
					}
					else
					{
						DestPoint = new PointF((float)(BoundRect.Width / 2), (float)(BoundRect.Height - 30));
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
						ItemList.ForEach(delegate(BaseItem x)
						{
							if (!(x is ShootingStarShard))
							{
								x.Obtain = true;
								x.Doubled = true;
							}
						});
					}
				}
				if (Story == null && Time > 70)
				{
					if (KClass.Key_Z)
					{
						if (TimeMain > 15)
						{
							Shoot();
						}
					}
					if (KClass.Key_X && !lastX)
					{
						if (EnchantmentState == EnchantmentType.Green)
						{
							var bulletRemover_Small = new BulletRemover_Small(StageData, OriginalPosition);
							bulletRemover_Small.Region = 500;
							ChangeEnchantment(EnchantmentType.None);
							EnemyPlaneList.ForEach(delegate(BaseEnemyPlane x)
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
			}
		}

		public void RetryClear()
		{
			EnchantmentTime = 0;
			Life = 2;
			Spell = 2;
			Power = 400;
			Score = (long)StageData.ContinueTimes;
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
			var baseEffect = new BaseEffect(StageData, "Extend", new PointF(0f, 0f), 0f, 1.5707963267948966)
			{
				Active = false,
				OriginalPosition = new PointF((float)(BoundRect.Width / 2), 112f),
				LifeTime = 90,
				TransparentValueF = 0f,
				Layer = 1
			};
			baseEffect.TransparentVelocityDictionary.Add(1, 13f);
			baseEffect.TransparentVelocityDictionary.Add(70, -13f);
		}

		public void SpellExtand()
		{
			StageData.SoundPlay("se_cardget.wav");
			Spell++;
			var baseEffect = new BaseEffect(StageData, "SpellExtend", new PointF(0f, 0f), 0f, 1.5707963267948966)
			{
				Active = false,
				OriginalPosition = new PointF((float)(BoundRect.Width / 2), 80f),
				LifeTime = 90,
				TransparentValueF = 0f,
				Layer = 1
			};
			baseEffect.TransparentVelocityDictionary.Add(1, 13f);
			baseEffect.TransparentVelocityDictionary.Add(70, -13f);
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
			if (Time >= DeadTime)
			{
				if (Time != 0)
				{
					if (Time < UnmatchedTime && Time / 2 % 2 == 0)
					{
						SpriteMain.Draw2D(TxtureObject, 1f, 0f, Position, Color.DarkBlue);
					}
					else
					{
						SpriteMain.Draw2D(TxtureObject, 1f, 0f, Position, Color.White);
					}
					for (var i = 0; i < PowerLevel; i++)
					{
						SubPlaneList[i].Show();
					}
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

		private const int unmatchedTime = 300;

		private const int deltaGreen = 120;

		private int life = 2;

		private int spell = 2;

		private int lifeChip = 0;

		private int spellChip = 0;

		public readonly int nextSpellChip = 4;

		public readonly int[] nextLifeChip = new int[] { 3, 5, 8, 10, 12, 15, 18, 20, 22 };

		private int lifeUpCount = 0;

		private int power = 100;

		private long score = 0L;

		private long hiScore = 0L;

		private bool lastX = false;

		private int starPoint = 0;

		public readonly int maxStarPoint = 3000;

		private int nextBluePoint = 300;

		private int nextGreenPoint = 3000;

		private int bluePoint = 0;

		private int greenPoint = 0;

		private EnchantmentType enchantmentState = EnchantmentType.None;

		private EnchantmentType lastColor = EnchantmentType.Green;

		private int enchantmentTime;

		private float rate = 1f;

		private int enchantmentCount = 0;

		private int enchantmentCountNeeded = 0;

		private bool OnRotate = false;

		private int Angle3D = 0;
	}
}
