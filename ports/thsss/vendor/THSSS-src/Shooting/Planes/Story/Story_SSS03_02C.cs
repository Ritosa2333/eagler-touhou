using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS03_02C : BaseStory_SSS
{
	public Story_SSS03_02C(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceSeiryuu_lo")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceSanae_sw")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 380f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("呜~被打败啦", CharLAcitve: false, CharRAcitve: true, "FaceSanae_hp", "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("好不甘心呀", CharLAcitve: false, CharRAcitve: true, "FaceSanae_hp", "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("那么你的流星碎片我就收下啦", CharLAcitve: true, CharRAcitve: false, "FaceSanae_hp", "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("这样我的信仰也会增加的吧？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_hp", "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("不过这样真的好吗？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("你指什么？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("你收集的流星碎片", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("那份光芒会让你迷失方向的哦？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("净说什么瞎话", CharLAcitve: true, CharRAcitve: false, "FaceSanae_lo", null));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			SBox.Dispose();
			Story = null;
			new EndStage(StageData, "St4", NextInit: false);
			new ClearBonus(StageData, 30000000);
		}
	}
}
