using System.Drawing;

namespace Shooting;

public class TransitionIn : BaseObject
{
	public int Delay { get; set; }

	public TransitionIn(StageDataPackage StageData)
	{
		base.StageData = StageData;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		OriginalPosition = new PointF(0f, 0f);
		TransparentValueF = 255f;
		StageData.InterfaceList.Add(this);
		Delay = 1;
		LifeTime = 50;
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time > Delay)
		{
			TransparentValueF -= 255 / (LifeTime - Delay);
		}
	}

	public override void Show()
	{
		SpriteMain.Draw2D(TxtureObject, 1f, 0f, OriginalPosition, TxtureObject.LeftTop, Color.FromArgb(TransparentValue, Color.Black));
	}
}
