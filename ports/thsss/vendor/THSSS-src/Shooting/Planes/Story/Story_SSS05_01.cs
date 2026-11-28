using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS05_01 : BaseStory_SSS
{
	private int circle = 120;

	private int tCount;

	public Story_SSS05_01(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\5-1A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\5-1B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\5-1C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\5-1D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		tCount++;
		if (Time < tCount / circle)
		{
			Time++;
		}
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			LastTime = Time;
		}
		else if (Time == MusicTime && LastTime < Time)
		{
			LastTime = Time;
		}
		if (Time == Conv.Count)
		{
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
