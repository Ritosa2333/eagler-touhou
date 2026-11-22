using System.Drawing;

namespace Shooting;

public class MyBorder_Line : Life_Line
{
	public MyBorder_Line(StageDataPackage StageData, string textureName, PointF OriginalPosition)
		: base(StageData, textureName, OriginalPosition)
	{
		size = 16f;
		maxLife = 11;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Life = MyPlane.EnchantmentCount;
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF(size, size);
		for (int i = 0; i < MyPlane.EnchantmentCountNeeded; i++)
		{
			TextureObject textureObject = TextureObjectDictionary["Border0d"];
			PointF position = new PointF(OriginalPosition.X + (float)(i * 12), OriginalPosition.Y);
			SpriteMain.Draw2D(textureObject.TXTure, textureObject.PosRect, destinationSize, new PointF(0f, 0f), 0f, position, Color.FromArgb(TransparentValue, Color.White));
		}
		for (int j = 0; j < Life; j++)
		{
			TextureObject textureObject2 = TextureObjectDictionary["Border"];
			PointF position2 = new PointF(OriginalPosition.X + (float)(j * 12), OriginalPosition.Y);
			SpriteMain.Draw2D(textureObject2.TXTure, textureObject2.PosRect, destinationSize, new PointF(0f, 0f), 0f, position2, Color.FromArgb(TransparentValue, Color.White));
		}
	}
}
