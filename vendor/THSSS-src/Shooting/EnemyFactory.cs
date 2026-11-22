using System.Drawing;

namespace Shooting;

public class EnemyFactory
{
	private readonly string[] EnemyType0 = new string[10] { "EnemyA0", "EnemyA1", "EnemyA2", "EnemyA3", "EnemyA4", "EnemyA5", "EnemyA6", "EnemyA7", "EnemyC0", "EnemyC1" };

	private readonly string[] EnemyType1 = new string[4] { "EnemyA8", "EnemyB1", "EnemyC2", "EnemyC3" };

	private readonly string[] EnemyType2 = new string[4] { "EnemyGhost0", "EnemyGhost1", "EnemyGhost2", "EnemyGhost3" };

	private readonly string[] EnemyType3 = new string[4] { "EnemyMaoYu00", "EnemyMaoYu01", "EnemyMaoYu02", "EnemyMaoYu03" };

	private readonly string[] EnemyType4 = new string[4] { "EnemyYYY4", "EnemyYYY5", "EnemyYYY6", "EnemyYYY7" };

	private int EnemyType = -1;

	private string EnemyName;

	public int HealthPoint;

	public int RedCount;

	public int BlueCount;

	public int GreenCount;

	public byte ColorType;

	public bool BackFire;

	public int ClearRadius;

	public bool StarFall;

	public EnemyFactory(string EnemyName)
	{
		this.EnemyName = EnemyName;
		if (ContainName(EnemyType0, EnemyName))
		{
			EnemyType = 0;
		}
		else if (ContainName(EnemyType1, EnemyName))
		{
			EnemyType = 1;
		}
		else if (ContainName(EnemyType2, EnemyName))
		{
			EnemyType = 2;
		}
		else if (ContainName(EnemyType3, EnemyName))
		{
			EnemyType = 3;
		}
		else if (ContainName(EnemyType4, EnemyName))
		{
			EnemyType = 4;
		}
	}

	private bool ContainName(string[] EnemyType, string EnemyName)
	{
		for (int i = 0; i < EnemyType.Length; i++)
		{
			if (EnemyType[i] == EnemyName)
			{
				return true;
			}
		}
		return false;
	}

	public BaseEnemyPlane_Touhou GenerateEnemy(StageDataPackage StageData)
	{
		BaseEnemyPlane_Touhou baseEnemyPlane_Touhou;
		switch (EnemyType)
		{
		case 0:
		{
			string enemyName = EnemyName;
			enemyName = enemyName.Replace("EnemyA", "");
			enemyName = enemyName.Replace("EnemyB", "");
			enemyName = enemyName.Replace("EnemyC", "");
			ColorType = byte.Parse(enemyName);
			ColorType = ((ColorType > 3) ? ((byte)(ColorType - 4)) : ColorType);
			baseEnemyPlane_Touhou = new EnemyPlane_TouhouSmall(StageData, EnemyName, new PointF(0f, 0f), 0f, 0.0, ColorType);
			if (BackFire)
			{
				((EnemyPlane_TouhouSmall)baseEnemyPlane_Touhou).SetBackFire();
			}
			break;
		}
		case 1:
			baseEnemyPlane_Touhou = new EnemyPlane_TouhouBig(StageData, EnemyName, new PointF(0f, 0f), 0f, 0.0, ColorType);
			break;
		case 2:
			ColorType = byte.Parse(EnemyName.Replace("EnemyGhost", ""));
			baseEnemyPlane_Touhou = new EnemyPlane_TouhouGhost(StageData, new PointF(0f, 0f), 0f, 0.0, ColorType);
			break;
		case 3:
			ColorType = byte.Parse(EnemyName.Replace("EnemyMaoYu0", ""));
			baseEnemyPlane_Touhou = new EnemyPlane_TouhouMaoYu(StageData, new PointF(0f, 0f), 0f, 0.0, ColorType);
			break;
		case 4:
			ColorType = (byte)(int.Parse(EnemyName.Replace("EnemyYYY", "")) - 4);
			baseEnemyPlane_Touhou = new EnemyPlane_TouhouYYY(StageData, new PointF(0f, 0f), 0f, 0.0, ColorType);
			break;
		default:
			baseEnemyPlane_Touhou = new EnemyPlane_TouhouNormal(StageData, EnemyName, new PointF(0f, 0f), 0f, 0.0, ColorType);
			break;
		}
		baseEnemyPlane_Touhou.HealthPoint = HealthPoint;
		baseEnemyPlane_Touhou.RedCount = RedCount;
		baseEnemyPlane_Touhou.BlueCount = BlueCount;
		baseEnemyPlane_Touhou.GreenCount = GreenCount;
		baseEnemyPlane_Touhou.ClearRadius = ClearRadius;
		baseEnemyPlane_Touhou.StarFall = StarFall;
		StageData.EnemyPlaneList.Remove(baseEnemyPlane_Touhou);
		return baseEnemyPlane_Touhou;
	}
}
