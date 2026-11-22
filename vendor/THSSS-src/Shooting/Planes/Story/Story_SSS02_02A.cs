namespace Shooting.Planes.Story;

internal class Story_SSS02_02A : BaseStory_SSS
{
	public Story_SSS02_02A(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation2(".\\Story\\2-2A.csv");
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			SBox.Dispose();
			Story = null;
			new EndStage(StageData, "St3", NextInit: false);
			new ClearBonus(StageData, 20000000);
		}
	}
}
