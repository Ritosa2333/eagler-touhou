using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS01_02B : BaseStory_SSS
{
	public Story_SSS01_02B(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, " ")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceMarisa_hp2")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 340f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("呼…轻松轻松~", CharLAcitve: true, CharRAcitve: false, null, null));
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
