namespace Shooting.Planes.Story;

internal class Story_SSS01_02A : BaseStory_SSS
{
	public Story_SSS01_02A(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation2(".\\Story\\1-2A.csv");
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			SBox.Dispose();
			Story = null;
			new EndStage(StageData, "St2", NextInit: false);
			new ClearBonus(StageData, 10000000);
		}
	}
}
