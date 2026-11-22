using System.Drawing;

namespace Shooting;

internal class BackgroundTransitionIn : BaseObject
{
	public int Delay { get; set; }

	public BackgroundTransitionIn(StageDataPackage StageData)
		: base(StageData, "bullet21_7", new PointF(0f, 0f), 0f, 0.0)
	{
		Background.BackgroundList.Add(this);
		Position = new PointF(StageData.BoundRect.X + StageData.BoundRect.Width / 2, StageData.BoundRect.Y + StageData.BoundRect.Height / 2);
		Scale = 30f;
		LifeTime = 480;
		Delay = 400;
		TransparentValueF = 255f;
		ColorValue = Color.Black;
	}

	public override void Ctrl()
	{
		Time++;
		if (Time > Delay)
		{
			TransparentValueF -= 255 / (LifeTime - Delay) + 1;
		}
	}
}
