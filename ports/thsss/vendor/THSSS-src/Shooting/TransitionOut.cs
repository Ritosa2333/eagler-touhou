using System.Drawing;

namespace Shooting;

public class TransitionOut : BaseObject
{
	public TransitionOut(StageDataPackage StageData)
	{
		base.StageData = StageData;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		OriginalPosition = new PointF(0f, 0f);
		TransparentValueF = 0f;
		StageData.InterfaceList.Add(this);
		LifeTime = 255;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		TransparentValueF += 255 / LifeTime;
	}

	public override void Show()
	{
		SpriteMain.Draw2D(TxtureObject, 1f, 0f, OriginalPosition, TxtureObject.LeftTop, Color.FromArgb(TransparentValue, Color.Black));
	}
}
