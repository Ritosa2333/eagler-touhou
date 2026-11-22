namespace Shooting.Planes.Story;

internal class Story_SSSEx_02 : BaseStory_SSS
{
	public Story_SSSEx_02(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation2(".\\Story\\Ex-2A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\Ex-2B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\Ex-2C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\Ex-2D.csv");
			break;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time != Conv.Count)
		{
			return;
		}
		SBox.Dispose();
		Story = null;
		new ClearBonus(StageData, 70000000);
		StageData.CurrentStageName = "ExAll";
		StageData.RepInfo.LastStage = StageData.CurrentStageName;
		if (!StageData.OnReplay)
		{
			StageData.C_History = StageData.PData.C_History.Find(StageData.FindClear);
			StageData.C_History.ClearTimes++;
			if (StageData.ContinueTimes == 0)
			{
				StageData.C_History.NoContinueClearTimes++;
			}
		}
		if (StageData.ContinueTimes == 0 && !StageData.OnReplay)
		{
			new EndStage(StageData, "FinalReplaySaveMenu", NextInit: true);
		}
		else
		{
			new EndStage(StageData, "GameSetMenu", NextInit: true);
		}
	}
}
