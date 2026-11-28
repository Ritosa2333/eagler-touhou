using System.Drawing;

namespace Shooting;

public class Life_Line : BaseInterface
{
	private int life;

	protected int maxLife = 9;

	public float size = 20f;

	public int Life
	{
		get
		{
			return life;
		}
		set
		{
			if (value > maxLife)
			{
				life = maxLife;
			}
			else if (value < 0)
			{
				life = 0;
			}
			else
			{
				life = value;
			}
		}
	}

	public Life_Line(StageDataPackage StageData, string textureName, PointF OriginalPosition)
		: base(StageData, textureName, OriginalPosition)
	{
	}

	public Life_Line(StageDataPackage StageData, TextureObject TxtureObject, PointF OriginalPosition)
		: base(StageData, null, OriginalPosition)
	{
		base.TxtureObject = TxtureObject;
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF(size, size);
		for (int i = 0; i < life; i++)
		{
			PointF position = new PointF(OriginalPosition.X + (float)i * size, OriginalPosition.Y);
			SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(0f, 0f), 0f, position, Color.FromArgb(TransparentValue, Color.White));
		}
	}
}
