namespace Shooting.Planes.Story;

internal class Story_SSS05_03 : BaseStory_SSS
{
	public Story_SSS05_03(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\5-3A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\5-3B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\5-3C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\5-3D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			SBox.Dispose();
			Story = null;
			new EndStage(StageData, "St6", NextInit: false);
			new ClearBonus(StageData, 50000000);
		}
	}
}
