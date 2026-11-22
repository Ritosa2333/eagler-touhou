using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS02_01 : BaseStory_SSS
{
	public Story_SSS02_01(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceRakuki_no")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "FaceAya_no")
		{
			TxtureObject2 = TextureObjectDictionary["FaceAya_base"]
		};
		SBox = new StoryBox(StageData);
		CharN = new CharacterName(StageData, "ename_Rakuki");
		new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
		Conv.Add(new Conversation("。。。", CharLAcitve: false, CharRAcitve: true, "FaceAya_no", "FaceRakuki_no"));
		Conv.Add(new Conversation("。。。。。。", CharLAcitve: true, CharRAcitve: false, "FaceAya_no", "FaceRakuki_no"));
		Conv.Add(new Conversation("四只老鼠在吹牛：", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_no2"));
		Conv.Add(new Conversation("甲：我每天都拿鼠药当糖吃；", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_hp"));
		Conv.Add(new Conversation("乙：我一天不踩老鼠夹脚发痒；", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_no"));
		Conv.Add(new Conversation("丙：我每天不过几次大街不踏实；", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_an"));
		Conv.Add(new Conversation("丁：时间不早了，回家抱猫去咯。", CharLAcitve: false, CharRAcitve: true, null, "FaceRakuki_bk"));
		Conv.Add(new Conversation("@画师，我需要一个泪奔的表情", CharLAcitve: true, CharRAcitve: false, "FaceAya_lo", null));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			Boss.Enabled = true;
			StageData.ChangeBGM(".\\BGM\\Boss02.wav", 0, 0, 255, 327663, 3504627);
			new MusicTitle(StageData, "疾风闪电", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(144, 255, 120));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
