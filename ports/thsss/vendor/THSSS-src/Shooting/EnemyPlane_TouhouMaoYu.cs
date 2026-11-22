using System.Drawing;

namespace Shooting;

internal class EnemyPlane_TouhouMaoYu : BaseEnemyPlane_Touhou
{
	public bool BackFire { get; set; }

	public Color BackFireColor { get; set; }

	public EnemyPlane_TouhouMaoYu(StageDataPackage StageData, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, null, OriginalPosition, Velocity, Direction, ColorType)
	{
		HealthPoint = 15f;
		Region = 6;
		TxtureObject = TextureObjectDictionary["EnemyMaoYu0" + ColorType];
		ItemCount = 1;
	}

	public override void Shoot()
	{
	}

	public override void TextureCtrl()
	{
		AngleDegree += 15.0;
	}
}
