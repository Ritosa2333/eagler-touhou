namespace Shooting.Planes.Story;

internal class Story_SSS04_02 : BaseStory_SSS
{
	public Story_SSS04_02(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\4-2A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\4-2B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\4-2C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\4-2D.csv");
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
			new EndStage(StageData, "St5", NextInit: false);
			new ClearBonus(StageData, 40000000);
		}
	}
}
