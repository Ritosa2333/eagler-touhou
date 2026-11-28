using System.Drawing;

namespace Shooting;

public class MySpell_Line : Life_Line
{
	public MySpell_Line(StageDataPackage StageData, string textureName, PointF OriginalPosition)
		: base(StageData, textureName, OriginalPosition)
	{
		size = 16f;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Life = MyPlane.Spell;
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF(size, size);
		for (int i = 0; i < 8; i++)
		{
			TextureObject textureObject = TextureObjectDictionary["Bomb0d"];
			PointF position = new PointF(OriginalPosition.X + (float)i * size, OriginalPosition.Y);
			SpriteMain.Draw2D(textureObject.TXTure, textureObject.PosRect, destinationSize, new PointF(0f, 0f), 0f, position, Color.FromArgb(TransparentValue, Color.White));
		}
		for (int j = 0; j < Life; j++)
		{
			TextureObject textureObject2 = TextureObjectDictionary["Bomb"];
			PointF position2 = new PointF(OriginalPosition.X + (float)j * size, OriginalPosition.Y);
			SpriteMain.Draw2D(textureObject2.TXTure, textureObject2.PosRect, destinationSize, new PointF(0f, 0f), 0f, position2, Color.FromArgb(TransparentValue, Color.White));
		}
		if (Life < 8)
		{
			int num = MyPlane.SpellChip * 4 / MyPlane.nextSpellChip;
			if (num > 0)
			{
				TextureObject textureObject3 = TextureObjectDictionary["Bomb" + (num - 1).ToString("D2")];
				PointF position3 = new PointF(OriginalPosition.X + (float)Life * size, OriginalPosition.Y);
				SpriteMain.Draw2D(textureObject3.TXTure, textureObject3.PosRect, destinationSize, new PointF(0f, 0f), 0f, position3, Color.FromArgb(TransparentValue, Color.White));
			}
		}
	}
}
