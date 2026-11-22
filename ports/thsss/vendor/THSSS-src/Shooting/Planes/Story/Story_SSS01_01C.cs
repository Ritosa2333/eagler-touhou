using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS01_01C : BaseStory_SSS
{
	public Story_SSS01_01C(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceAmi01")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceSanae_hp")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 380f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("那边的人类 请等一下！", CharLAcitve: false, CharRAcitve: true, "FaceSanae_hp", "FaceAmi01"));
		Conv.Add(new Conversation("哇 是妖精呢~", CharLAcitve: true, CharRAcitve: false, "FaceSanae_hp", null));
		Conv.Add(new Conversation("有什么事情吗？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no2", null));
		Conv.Add(new Conversation("我们来开party吧！", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi05"));
		Conv.Add(new Conversation("party？现在？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("是啊是啊 我还要邀请很多人呢", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi07"));
		Conv.Add(new Conversation("你也一起来吧~", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi02"));
		Conv.Add(new Conversation("真是有活力呢 妖精晚上都不睡觉的吗……", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("真遗憾呢，我还有更重要的事情要调查才行", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no", null));
		Conv.Add(new Conversation("咦？忙着赶路吗？", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi01"));
		Conv.Add(new Conversation("那也没关系", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi04"));
		Conv.Add(new Conversation("party现在开始也是可以的哦！", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi05"));
		Conv.Add(new Conversation("妨碍的话我会揍你的哦？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("在这充满魔力的流星的夜晚", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi04"));
		Conv.Add(new Conversation("让我们用弹幕", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi01"));
		Conv.Add(new Conversation("喧闹起来吧！", CharLAcitve: false, CharRAcitve: true, null, "FaceAmi05"));
		Conv.Add(new Conversation("这可不行呢，看来有必要退治一下了！", CharLAcitve: true, CharRAcitve: false, "FaceSanae_an2", null));
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
		if (Time == 15 && LastTime < Time)
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
