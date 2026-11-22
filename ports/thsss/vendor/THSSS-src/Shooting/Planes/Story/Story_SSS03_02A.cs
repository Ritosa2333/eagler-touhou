namespace Shooting.Planes.Story;

internal class Story_SSS03_02A : BaseStory_SSS
{
	public Story_SSS03_02A(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation2(".\\Story\\3-2A.csv");
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			SBox.Dispose();
			Story = null;
			new EndStage(StageData, "St4", NextInit: false);
			new ClearBonus(StageData, 30000000);
		}
	}
}
