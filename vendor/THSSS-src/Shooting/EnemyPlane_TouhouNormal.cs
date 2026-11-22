using System.Drawing;

namespace Shooting;

internal class EnemyPlane_TouhouNormal : BaseEnemyPlane_Touhou
{
	public EnemyPlane_TouhouNormal(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: this(StageData, EnemyName, OriginalPosition, Velocity, Direction)
	{
		base.ColorType = ColorType;
	}

	public EnemyPlane_TouhouNormal(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, null, OriginalPosition, Velocity, Direction)
	{
		TxtureObject = TextureObjectDictionary[EnemyName];
	}

	public override void TextureCtrl()
	{
	}
}
