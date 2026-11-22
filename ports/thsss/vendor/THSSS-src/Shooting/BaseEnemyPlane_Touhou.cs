using System;
using System.Collections.Generic;
using System.Drawing;
using SlimDX;

namespace Shooting;

public class BaseEnemyPlane_Touhou : BaseEnemyPlane
{
	private TextureObject[,] TxtureObjects { get; set; }

	private float IndexX { get; set; }

	private int IndexY { get; set; }

	public byte ColorType { get; set; }

	public int ItemCount
	{
		set
		{
			RedCount = value;
			BlueCount = value;
		}
	}

	public int RedCount { get; set; }

	public int BlueCount { get; set; }

	public int GreenCount { get; set; }

	public int ClearRadius { get; set; }

	public bool StarFall { get; set; }

	public BaseEnemyPlane_Touhou()
	{
	}

	public BaseEnemyPlane_Touhou(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: this(StageData, EnemyName, OriginalPosition, Velocity, Direction)
	{
		this.ColorType = ColorType;
	}

	public BaseEnemyPlane_Touhou(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, null, OriginalPosition, Velocity, Direction)
	{
		base.OriginalPosition = OriginalPosition;
		if (EnemyName != null)
		{
			TxtureObjects = new TextureObject[2, 8]
			{
				{
					TextureObjectDictionary[EnemyName + "_0"],
					TextureObjectDictionary[EnemyName + "_1"],
					TextureObjectDictionary[EnemyName + "_2"],
					TextureObjectDictionary[EnemyName + "_3"],
					TextureObjectDictionary[EnemyName + "_0"],
					TextureObjectDictionary[EnemyName + "_1"],
					TextureObjectDictionary[EnemyName + "_2"],
					TextureObjectDictionary[EnemyName + "_3"]
				},
				{
					TextureObjectDictionary[EnemyName + "_4"],
					TextureObjectDictionary[EnemyName + "_5"],
					TextureObjectDictionary[EnemyName + "_6"],
					TextureObjectDictionary[EnemyName + "_7"],
					TextureObjectDictionary[EnemyName + "_8"],
					TextureObjectDictionary[EnemyName + "_9"],
					TextureObjectDictionary[EnemyName + "_A"],
					TextureObjectDictionary[EnemyName + "_B"]
				}
			};
		}
		EventGroupList = new List<EventGroup>();
		EventsExecutionList = new List<Execution>();
	}

	public override void Ctrl()
	{
		base.Ctrl();
		TextureCtrl();
	}

	public virtual void TextureCtrl()
	{
		AngleWithDirection = false;
		Angle = Math.PI / 2.0;
		int num = (Mirrored ? IndexY : (-IndexY));
		if (Vx < -0.5f)
		{
			Mirrored = true;
			IndexY = 1;
			if (IndexX < 3f)
			{
				IndexX += 1f / 6f;
			}
			else
			{
				IndexX = 3 + Time % 24 / 6;
			}
		}
		else if (Vx > 0.5f)
		{
			Mirrored = false;
			IndexY = 1;
			if (IndexX < 3f)
			{
				IndexX += 1f / 6f;
			}
			else
			{
				IndexX = 3 + Time % 24 / 6;
			}
		}
		else
		{
			Mirrored = false;
			IndexY = 0;
			IndexX = Time % 48 / 6;
		}
		if (num != (Mirrored ? IndexY : (-IndexY)))
		{
			IndexX = 0f;
		}
		if (IndexX > 7f)
		{
			IndexX = 7f;
		}
		TxtureObject = TxtureObjects[IndexY, (int)IndexX];
	}

	public override void Shoot()
	{
	}

	public override void GiveEndEffect()
	{
		//IL_0262: Unknown result type (might be due to invalid IL or missing references)
		Color color;
		switch (ColorType)
		{
		case 0:
			color = Color.Red;
			new EndEnemy(StageData, "EndEnemy0", Position, 0f, 0.0).Scale = 0.2f;
			break;
		case 1:
			color = Color.CornflowerBlue;
			new EndEnemy(StageData, "EndEnemy1", Position, 0f, 0.0).Scale = 0.2f;
			break;
		case 2:
			color = Color.Green;
			new EndEnemy(StageData, "EndEnemy2", Position, 0f, 0.0).Scale = 0.2f;
			break;
		case 3:
			color = Color.Yellow;
			new EndEnemy(StageData, "EndEnemy3", Position, 0f, 0.0).Scale = 0.2f;
			break;
		default:
			color = Color.White;
			new EndEnemy(StageData, "EndEnemy2", Position, 0f, 0.0).Scale = 0.2f;
			new EndEnemy(StageData, "EndEnemy3", Position, 0f, 0.0).Scale = 0.2f;
			break;
		}
		for (int i = 0; i < 10; i++)
		{
			new Particle3D3(StageData, "Effect-0", OriginalPosition, (float)Ran.Next(40) / 10f, (float)Ran.Next(360) / 180f * (float)Math.PI, 40)
			{
				Angle = (float)StageData.Ran.Next(0, 360) * (float)Math.PI / 180f,
				AngularVelocity3D = (float)StageData.Ran.Next(10, 30) / 100f,
				Scale = (float)StageData.Ran.Next(12, 18) / 10f,
				RotatingAxis = new Vector3((float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100), (float)StageData.Ran.Next(-100, 100)),
				ColorValue = color,
				MaxTransparent = 200
			};
		}
	}

	public override void GiveItems()
	{
		for (int i = 0; i < RedCount; i++)
		{
			PointF originalPosition = new PointF(OriginalPosition.X + (float)Ran.Next(-RedCount * 5, RedCount * 5), OriginalPosition.Y + (float)Ran.Next(-RedCount * 5, RedCount * 5));
			new PowerItem_Touhou(StageData, originalPosition);
		}
		for (int j = 0; j < BlueCount; j++)
		{
			PointF originalPosition2 = new PointF(OriginalPosition.X + (float)Ran.Next(-BlueCount * 5, BlueCount * 5), OriginalPosition.Y + (float)Ran.Next(-BlueCount * 5, BlueCount * 5));
			new ScoreItem_Touhou(StageData, originalPosition2);
		}
		for (int k = 0; k < GreenCount; k++)
		{
			PointF originalPosition3 = new PointF(OriginalPosition.X + (float)Ran.Next(-GreenCount * 5, GreenCount * 5), OriginalPosition.Y + (float)Ran.Next(-GreenCount * 5, GreenCount * 5));
			new BeliefItem_Touhou(StageData, originalPosition3);
		}
		if (ClearRadius != 0)
		{
			new BulletRemover_Small(StageData, OriginalPosition)
			{
				Region = ClearRadius,
				LifeTime = 60
			};
		}
		if (StarFall)
		{
			new BaseEffect(StageData, "StarShardBS", Position, 8f, -Math.PI / 2.0)
			{
				GhostingCount = 40,
				AngularVelocityDegree = 5f
			};
			new ShootingStarShard(StageData, new PointF(OriginalPosition.X, 0f));
		}
	}

	public override void EventCtrl()
	{
		EventGroupList.ForEach((EventGroup a) =>
		{
			a.Update(this);
		});
		EventsExecutionList.ForEach((Execution a) =>
		{
			a.Update(this);
		});
	}
}
