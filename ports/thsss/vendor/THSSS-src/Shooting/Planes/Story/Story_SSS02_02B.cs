using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS02_02B : BaseStory_SSS
{
	public Story_SSS02_02B(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceRakuki_lo")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceMarisa_hp")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 340f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("怎么可能…我竟然会败……", CharLAcitve: false, CharRAcitve: true, "FaceMarisa_hp", "FaceRakuki_lo"));
		Conv.Add(new Conversation("对不起…姐姐大人……", CharLAcitve: false, CharRAcitve: true, null, null));
		Conv.Add(new Conversation("呼~总算是通过了", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp", null));
		Conv.Add(new Conversation("Lucky Lucky~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp2", null));
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
