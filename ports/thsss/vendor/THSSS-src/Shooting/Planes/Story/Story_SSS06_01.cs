using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS06_01 : BaseStory_SSS
{
	public Story_SSS06_01(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\6-1A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\6-1B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\6-1C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\6-1D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			CharN = new CharacterName(StageData, "ename_Tensei");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
			LastTime = Time;
		}
		else if (Time == MusicTime && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\Boss06.wav", 0, 0, 255, 1697409, 10789065);
			LastTime = Time;
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "？？？？？", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Boss.Enabled = true;
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.SkyBlue);
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
