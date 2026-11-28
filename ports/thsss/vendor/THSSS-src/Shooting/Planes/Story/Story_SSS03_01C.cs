using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS03_01C : BaseStory_SSS
{
	public Story_SSS03_01C(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceSeiryuu_no")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceSanae_no")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 380f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("不知不觉已经进入密林当中了吗？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no", "FaceSeiryuu_sp"));
		Conv.Add(new Conversation("呜……视线都被遮挡住了 这样就没法看到流星了呢", CharLAcitve: true, CharRAcitve: false, "FaceSanae_lo", null));
		Conv.Add(new Conversation("不赶紧飞上树林上方的话……", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("咦？是人类吗", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sp"));
		Conv.Add(new Conversation("人类为何会来这片偏远的柳林？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_hp"));
		Conv.Add(new Conversation("那个 我是来调查这些流星碎片的", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no2", null));
		Conv.Add(new Conversation("总觉得要是收集到这份力量的话", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no", null));
		Conv.Add(new Conversation("信仰也会增加的吧", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no", null));
		Conv.Add(new Conversation("是这样啊~", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_hp"));
		Conv.Add(new Conversation("的确呢 能够感受到这些星星里面包含着充足的魔力", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_dp"));
		Conv.Add(new Conversation("你知道是谁在制造这些流星吗？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no", null));
		Conv.Add(new Conversation("不知道呢", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_no"));
		Conv.Add(new Conversation("不过如果吸收掉的话", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_hp"));
		Conv.Add(new Conversation("我的力量也会增长的吧", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sw"));
		Conv.Add(new Conversation("你是妖怪？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sp", null));
		Conv.Add(new Conversation("你觉得会有孤零零的人类少女来到这偏远的地方吗", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sw"));
		Conv.Add(new Conversation("你也在收集流星碎片吗？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_an2", null));
		Conv.Add(new Conversation("原来是竞争对手呢", CharLAcitve: true, CharRAcitve: false, "FaceSanae_an", null));
		Conv.Add(new Conversation("区区人类也要与妖怪竞争吗？被小看了啊", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_dp"));
		Conv.Add(new Conversation("怎么说我也是存在了千年的妖怪了", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_dp"));
		Conv.Add(new Conversation("妖怪的厉害之处", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sw"));
		Conv.Add(new Conversation("就让你好好体会一下吧！", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sw"));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 15 && LastTime < Time)
		{
			CharN = new CharacterName(StageData, "ename_Seiryuu");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
			LastTime = Time;
		}
		else if (Time == 20 && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\Boss03.wav", 0, 0, 255, 1286397, 6275430);
			LastTime = Time;
		}
		else if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "青柳传说", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Boss.Enabled = true;
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(40, 255, 20));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
