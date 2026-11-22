using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS01_02 : BaseStory_SSS
{
	public Story_SSS01_02(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceAmi01")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceAya_no")
		{
			TxtureObject2 = TextureObjectDictionary["FaceAya_base"]
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("。。。", CharLAcitve: true, CharRAcitve: false, "FaceAya_no", null));
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
