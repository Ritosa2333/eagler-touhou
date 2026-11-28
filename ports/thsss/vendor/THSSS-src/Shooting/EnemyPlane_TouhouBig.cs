using System.Drawing;

namespace Shooting;

internal class EnemyPlane_TouhouBig : BaseEnemyPlane_Touhou
{
	public EnemyPlane_TouhouBig(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, EnemyName, OriginalPosition, Velocity, Direction, ColorType)
	{
		HealthPoint = 600f;
		ItemCount = 4;
		Region = 10;
	}

	public EnemyPlane_TouhouBig(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, EnemyName, OriginalPosition, Velocity, Direction)
	{
		HealthPoint = 600f;
		ItemCount = 4;
		Region = 10;
	}
}
