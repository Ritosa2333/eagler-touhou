using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS04_01 : BaseStory_SSS
{
	public Story_SSS04_01(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\4-1A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\4-1B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\4-1C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\4-1D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			CharN = new CharacterName(StageData, "ename_Kage");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
			LastTime = Time;
		}
		else if (Time == MusicTime && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\Boss04.wav", 0, 0, 255, 463491, 7541982);
			LastTime = Time;
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "镜中的幻象", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Boss.Enabled = true;
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.Yellow);
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
