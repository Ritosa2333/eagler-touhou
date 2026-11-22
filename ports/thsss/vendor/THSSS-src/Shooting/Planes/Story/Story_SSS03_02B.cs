using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS03_02B : BaseStory_SSS
{
	public Story_SSS03_02B(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceSeiryuu_lo")
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
		Conv.Add(new Conversation("呜~被打败啦", CharLAcitve: false, CharRAcitve: true, "FaceMarisa_hp2", "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("好不甘心呀", CharLAcitve: false, CharRAcitve: true, null, null));
		Conv.Add(new Conversation("那么 你的那份星之碎片就给我保管咯~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp2", "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("前方侦测到高能量魔力反应 会是什么呢？", CharLAcitve: true, CharRAcitve: false, null, null));
		Conv.Add(new Conversation("前进前进~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp", null));
		Conv.Add(new Conversation("小心好奇害死猫哦", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_lo"));
		Conv.Add(new Conversation("猫也是有九条命的呢", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp2", null));
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
