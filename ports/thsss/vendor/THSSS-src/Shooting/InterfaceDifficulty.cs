using System.Drawing;

namespace Shooting;

public class InterfaceDifficulty : BaseInterface
{
	public InterfaceDifficulty(StageDataPackage StageData, PointF OriginalPosition)
		: base(StageData, null, OriginalPosition)
	{
	}

	public override void Ctrl()
	{
		base.Ctrl();
		switch (StageData.Difficulty)
		{
		case DifficultLevel.Easy:
			TxtureObject = TextureObjectDictionary["Easy"];
			break;
		case DifficultLevel.Normal:
			TxtureObject = TextureObjectDictionary["Normal"];
			break;
		case DifficultLevel.Hard:
			TxtureObject = TextureObjectDictionary["Hard"];
			break;
		case DifficultLevel.Lunatic:
			TxtureObject = TextureObjectDictionary["Lunatic"];
			break;
		case DifficultLevel.Extra:
			TxtureObject = TextureObjectDictionary["Extra"];
			break;
		case DifficultLevel.Ultra:
			break;
		}
	}

	public override void Show()
	{
		if (TxtureObject != null)
		{
			SpriteMain.Draw2D(TxtureObject, 1f, 0f, OriginalPosition, Color.FromArgb(TransparentValue, Color.White));
		}
	}
}
