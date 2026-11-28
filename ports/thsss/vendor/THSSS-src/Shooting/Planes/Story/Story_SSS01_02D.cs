namespace Shooting.Planes.Story;

internal class Story_SSS01_02D : BaseStory_SSS
{
	public Story_SSS01_02D(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation(".\\Story\\1-2D.csv");
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
