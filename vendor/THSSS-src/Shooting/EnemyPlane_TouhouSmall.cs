using System.Drawing;

namespace Shooting;

internal class EnemyPlane_TouhouSmall : BaseEnemyPlane_Touhou
{
	protected bool BackFire { get; set; }

	protected Color BackFireColor { get; set; }

	public EnemyPlane_TouhouSmall(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction, byte ColorType)
		: base(StageData, EnemyName, OriginalPosition, Velocity, Direction, ColorType)
	{
		HealthPoint = 10f;
		ItemCount = 1;
	}

	public EnemyPlane_TouhouSmall(StageDataPackage StageData, string EnemyName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, EnemyName, OriginalPosition, Velocity, Direction)
	{
		HealthPoint = 20f;
		ItemCount = 1;
	}

	public override void Show()
	{
		if (BackFire)
		{
			TextureObject txtureObject = TextureObjectDictionary["GhostFire_" + Time % 32 / 4];
			SpriteMain.Draw2D(txtureObject, ScaleWidth, ScaleLength, 0f, Position, Color.FromArgb(192, BackFireColor), Mirrored);
		}
		base.Show();
	}

	public void SetBackFire()
	{
		BackFire = true;
		switch (ColorType)
		{
		case 0:
			BackFireColor = Color.Red;
			break;
		case 1:
			BackFireColor = Color.Blue;
			break;
		case 2:
			BackFireColor = Color.Green;
			break;
		case 3:
			BackFireColor = Color.Yellow;
			break;
		default:
			BackFireColor = Color.White;
			break;
		}
	}
}
