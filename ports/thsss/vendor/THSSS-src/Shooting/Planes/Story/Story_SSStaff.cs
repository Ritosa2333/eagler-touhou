namespace Shooting.Planes.Story;

internal class Story_SSStaff : BaseStory_SSS
{
	private CG Cg;

	public Story_SSStaff(StageDataPackage StageData)
		: base(StageData)
	{
		switch (MyPlane.Name)
		{
		case "Reimu":
			LoadConversation(".\\Story\\Staff-1A.csv");
			break;
		case "Marisa":
			LoadConversation(".\\Story\\Staff-1B.csv");
			break;
		case "Sanae":
			LoadConversation(".\\Story\\Staff-1C.csv");
			break;
		case "Koishi":
			LoadConversation(".\\Story\\Staff-1D.csv");
			break;
		}
		SBox.MaxTransparent = 0;
		Cg = new CG(StageData, null);
	}

	public override void Ctrl()
	{
		base.Ctrl();
		CharL.TxtureObject = null;
		CharR.TxtureObject = null;
		Cg.Ctrl();
		if (Time < Conv.Count)
		{
			Cg.TxtureObject = TextureObjectDictionary[Conv[Time].CharL];
		}
		if (Time == Conv.Count)
		{
			SBox.Dispose();
			Story = null;
		}
	}

	public override void Show()
	{
		Cg.Show();
		base.Show();
	}
}
