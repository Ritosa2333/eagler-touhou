using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS02_01C : BaseStory_SSS
{
	public Story_SSS02_01C(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceRakuki_no")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceSanae_an")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(70f, 380f)
		};
		SBox = new StoryBox(StageData);
		Conv.Add(new Conversation("前方的区域禁止进入", CharLAcitve: false, CharRAcitve: true, "FaceSanae_an", "FaceRakuki_no"));
		Conv.Add(new Conversation("你是谁 为什么要阻拦我？", CharLAcitve: true, CharRAcitve: false, "FaceSanae_an", "FaceRakuki_no"));
		Conv.Add(new Conversation("我叫洛熙", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_an2"));
		Conv.Add(new Conversation("你是人类？回去吧 这些与你无关", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_no"));
		Conv.Add(new Conversation("果然你们在谋划些什么", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no", null));
		Conv.Add(new Conversation("我是现人神 可不能坐视不管", CharLAcitve: true, CharRAcitve: false, "FaceSanae_no2", null));
		Conv.Add(new Conversation("现人神也是一样", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_an2"));
		Conv.Add(new Conversation("只要争取到足够的时间的话", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_dp"));
		Conv.Add(new Conversation("我们自然会离开这里", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_no2"));
		Conv.Add(new Conversation("这怎么可以！", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("再放任这些魔力的流星不管的话", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("各路妖怪们都会暴动的", CharLAcitve: true, CharRAcitve: false, "FaceSanae_sw", null));
		Conv.Add(new Conversation("妖怪会如何与我人类何干？", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_no"));
		Conv.Add(new Conversation("看来只能强行突破了", CharLAcitve: true, CharRAcitve: false, "FaceSanae_an", null));
		Conv.Add(new Conversation("来吧 弹幕对战！", CharLAcitve: true, CharRAcitve: false, "FaceSanae_an2", null));
		Conv.Add(new Conversation("天真！", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_an"));
		Conv.Add(new Conversation("你真以为你能逃过我的箭雨吗？", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_an"));
		Conv.Add(new Conversation("在这把琴引山的神弓下", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_dp"));
		Conv.Add(new Conversation("去死吧！", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_bk"));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 2 && LastTime < Time)
		{
			LastTime = Time;
			CharN = new CharacterName(StageData, "ename_Rakuki");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
		}
		if (Time == 17 && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\Boss02.wav", 0, 0, 255, 327663, 3504627);
			LastTime = Time;
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "疾风闪电", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Boss.Enabled = true;
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(144, 255, 120));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
