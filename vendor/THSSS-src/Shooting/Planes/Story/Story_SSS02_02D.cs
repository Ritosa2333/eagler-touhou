namespace Shooting.Planes.Story;

internal class Story_SSS02_02D : BaseStory_SSS
{
	public Story_SSS02_02D(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation(".\\Story\\2-2D.csv");
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
