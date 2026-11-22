using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS01_01B : BaseStory_SSS
{
	public Story_SSS01_01B(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceAmi05")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceMarisa_sp")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 340f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("流星碎片的确是掉落在这附近了……", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_sp", "FaceAmi05"));
		Conv.Add(new Conversation("没找到啊……", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_lo", null));
		Conv.Add(new Conversation("出现了~可以邀请到Party的人类~", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi05"));
		Conv.Add(new Conversation("哈？搞什么 原来是只妖精啊", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_sw", null));
		Conv.Add(new Conversation("别飞那么快嘛 来参加热闹的Party吧！", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi03"));
		Conv.Add(new Conversation("不好意思 我可是很忙的呐", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp", null));
		Conv.Add(new Conversation("妨碍我的话会狠狠揍你的哦", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp2", null));
		Conv.Add(new Conversation("你很忙吗？", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi05"));
		Conv.Add(new Conversation("很忙的哟", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_no", null));
		Conv.Add(new Conversation("啊啊…虽然有点早 不过现在就开始吧", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi06"));
		Conv.Add(new Conversation("为大忙人准备的Party！", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi04"));
		Conv.Add(new Conversation("呀咧呀咧…果然还是要揍一顿ZE~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_sw", null));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 3 && LastTime < Time)
		{
			LastTime = Time;
			CharN = new CharacterName(StageData, "ename_Ami");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
		}
		if (Time == 10 && LastTime < Time)
		{
			LastTime = Time;
			StageData.ChangeBGM(".\\BGM\\Boss01.wav", 0, 0, 255, 754110, 3294270);
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "喧闹吧！在这不眠之夜", new Point(BoundRect.Width, BoundRect.Height - 16))
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
