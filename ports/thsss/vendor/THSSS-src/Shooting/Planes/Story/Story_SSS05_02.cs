using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS05_02 : BaseStory_SSS
{
	public Story_SSS05_02(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\5-2A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\5-2B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\5-2C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\5-2D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			LastTime = Time;
			CharN = new CharacterName(StageData, "ename_Rakukun");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
		}
		else if (Time == MusicTime && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\Boss05.wav", 0, 0, 255, 1000056, 6599345);
			LastTime = Time;
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "引燃夜空的星火", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Boss.Enabled = true;
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(144, 255, 120));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpColorKey = 2;
			Background3D.WarpEnabled = true;
		}
	}
}
