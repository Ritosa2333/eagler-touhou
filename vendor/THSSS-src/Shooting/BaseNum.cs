using System.Drawing;

namespace Shooting;

internal class BaseNum : BaseObject
{
	private const int NumWidth = 16;

	public int Num { get; set; }

	public int Count { get; set; }

	public BaseNum(StageDataPackage StageData, PointF Position)
		: base(StageData, null, Position, 0f, 0.0)
	{
	}

	public override void Show()
	{
		int num = Num;
		for (int i = 0; i < Count; i++)
		{
			int num2 = num % 10;
			num /= 10;
			TextureObject textureObject = TextureObjectDictionary[num2.ToString()];
			SpriteMain.Draw2D(textureObject.TXTure, textureObject.PosRect, new SizeF(textureObject.Width, textureObject.Height), textureObject.RotatingCenter, 0f, new PointF(Position.X + (float)(16 * (Count - i - 1)), Position.Y), Color.White);
		}
	}
}
