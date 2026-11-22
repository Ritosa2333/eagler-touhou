using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS03_01B : BaseStory_SSS
{
	public Story_SSS03_01B(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceSeiryuu_no")
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
		Conv.Add(new Conversation("碎片掉到这片密林里去了呢…", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_sp", "FaceSeiryuu_sp"));
		Conv.Add(new Conversation("不过话说这附近的树木也太茂密了吧…不方便收集呢", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_lo", null));
		Conv.Add(new Conversation("不赶紧到视野好一点的地方去的话……", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_lo", null));
		Conv.Add(new Conversation("你是…人类？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sp"));
		Conv.Add(new Conversation("暂时还是~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp", null));
		Conv.Add(new Conversation("人类为何要来这偏远的柳林呢？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_no"));
		Conv.Add(new Conversation("本来是顺着魔力来收集星星碎片的说…然后…", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_no2", null));
		Conv.Add(new Conversation("迷路了~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp2", null));
		Conv.Add(new Conversation("哈？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sw"));
		Conv.Add(new Conversation("那个 方便的话 可以指个路么ZE~", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_hp", null));
		Conv.Add(new Conversation("诶嘿~可以的哟~", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_no"));
		Conv.Add(new Conversation("不过作为交换", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_hp"));
		Conv.Add(new Conversation("把你手中的流星碎片留下吧", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_dp"));
		Conv.Add(new Conversation("只有这个不可以呢", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_no", null));
		Conv.Add(new Conversation("你该不会一开始就看准了这个吧？", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_sw", null));
		Conv.Add(new Conversation("柳树的妖怪？", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_sw", null));
		Conv.Add(new Conversation("被识破了吗？", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_no"));
		Conv.Add(new Conversation("你也不是简单的人类啊", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_dp"));
		Conv.Add(new Conversation("人类少女怎么会孤零零地来这偏远的柳林", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_no", null));
		Conv.Add(new Conversation("把你手中的八卦炉作为交换也是可以的哦~", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sp"));
		Conv.Add(new Conversation("这个也不行！", CharLAcitve: true, CharRAcitve: false, "FaceMarisa_an", null));
		Conv.Add(new Conversation("是吗？那么……", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_dp"));
		Conv.Add(new Conversation("就把你的命留下吧！", CharLAcitve: false, CharRAcitve: true, null, "FaceSeiryuu_sw"));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == 16 && LastTime < Time)
		{
			CharN = new CharacterName(StageData, "ename_Seiryuu");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
			LastTime = Time;
		}
		else if (Time == 21 && LastTime < Time)
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
