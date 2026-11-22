using System.Drawing;

namespace Shooting;

internal class SelectBox : BaseEffect
{
	public SelectBox(StageDataPackage StageData)
		: base(StageData)
	{
		TxtureObject = TextureObjectDictionary["selectframe2"];
	}

	public override void Show()
	{
		SpriteMain.Draw2D(TxtureObject.TXTure, new Rectangle((int)OriginalPosition.X, (int)OriginalPosition.Y, 512, 22), new SizeF(512f, 22f), new PointF(0f, 0f), 0f, OriginalPosition, Color.White);
	}
}
