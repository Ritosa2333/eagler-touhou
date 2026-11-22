using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSSEx_01 : BaseStory_SSS
{
	public Story_SSSEx_01(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\Ex-1A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\Ex-1B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\Ex-1C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\Ex-1D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			CharN = new CharacterName(StageData, "ename_Rika");
			LastTime = Time;
		}
		else if (Time == MusicTime && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\BossEx.wav", 0, 0, 255, 1125873, 10783332);
			LastTime = Time;
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "风中花，雪中月", new Point(BoundRect.Width, BoundRect.Height - 16))
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
