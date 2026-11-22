using System.Drawing;

namespace Shooting;

public class BaseInterface : BaseObject
{
	public int Delay { get; set; }

	public BaseInterface(StageDataPackage StageData, string textureName, PointF OriginalPosition)
	{
		base.StageData = StageData;
		if (textureName != null)
		{
			TxtureObject = TextureObjectDictionary[textureName];
		}
		base.OriginalPosition = OriginalPosition;
		TransparentVelocity = 10f;
	}

	public BaseInterface()
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time <= Delay)
		{
			TransparentValueF = 0f;
		}
	}

	public override void Show()
	{
		SpriteMain.Draw2D(TxtureObject, 1f, 0f, OriginalPosition, TxtureObject.LeftTop, Color.FromArgb(TransparentValue, ColorValue));
	}
}
