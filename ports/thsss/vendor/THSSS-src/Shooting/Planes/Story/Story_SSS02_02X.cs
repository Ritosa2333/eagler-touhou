using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS02_02X : BaseStory_SSS
{
	public Story_SSS02_02X(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceRakuki_lo")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "Myplane")
		{
			TxtureObject2 = TextureObjectDictionary["Myplane"]
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("。。。", CharLAcitve: false, CharRAcitve: true, "Myplane", "FaceRakuki_lo"));
		Conv.Add(new Conversation("这是意外。", CharLAcitve: true, CharRAcitve: false, "Myplane", "FaceRakuki_lo"));
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
