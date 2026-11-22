using System.Drawing;

namespace Shooting;

public class TextTwinkle : BaseEffect
{
	public int Circle { get; set; }

	public Color TwinkleColor { get; set; }

	public Color OriginalColor { get; set; }

	public TextTwinkle(StageDataPackage StageData, string textureName, PointF Position, float Velocity, double Direction)
		: base(StageData, textureName, Position, Velocity, Direction)
	{
		Active = false;
		LifeTime = 150;
		Circle = 30;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time <= 1)
		{
			OriginalColor = ColorValue;
		}
		else if (Time < LifeTime * 3 / 5)
		{
			if (Time % Circle == 0)
			{
				ColorValue = TwinkleColor;
			}
			else if (Time % Circle == Circle / 2)
			{
				ColorValue = OriginalColor;
			}
		}
		else if (LifeTime * 4 / 5 < Time && Time < LifeTime)
		{
			TransparentValueF -= MaxTransparent * 5 / LifeTime;
		}
	}
}
